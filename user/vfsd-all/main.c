#include <horizon/types.h>
#include <horizon/ipc.h>
#include <sys/sched.h>
#include <sys/msg.h>
#include <sys/svc.h>
#include <malloc.h>
#include <string.h>
#include <memory.h>
#include <stdint.h>
#include <stdbool.h>

#include "fs.h"
#include "../procd-all/pnotify.h"

typedef struct vfs_node vfs_node_t;
struct vfs_node
{
	char name[32];
	ipcport_t owner;

	size_t uid;
	unsigned type;

	vfs_node_t* next;
	vfs_node_t* children;
};

typedef struct file_table file_table_t;
struct file_table
{
	size_t count;
	vfs_node_t* nodes[8];
};

vfs_node_t root_node = {0};

ipcport_t procmgr = 0;
file_table* file_tables = NULL;

vfs_node_t* add_node(vfs_node_t* parent, const char* name)
{
	vfs_node_t* node = malloc(sizeof(vfs_node_t));
	memset(node, 0, sizeof(vfs_node_t));

	strcpy(node->name, name);
	node->owner = parent->owner;

	node->next = parent->children;
	parent->children = node;

	return node;
}

vfs_node_t* request_node(vfs_node_t* parent, const char* name, bool virt)
{
	if (virt)
	{
		if (parent->type != VFS_VIRTDIR)
			return NULL;

		vfs_node_t* node = add_node(parent, name);
		node->type = VFS_VIRTDIR;

		return node;
	}

	struct msg request = {{0}};
	request.to = parent->owner;

	request.code = VFS_FSFIND;
	request.args[0] = parent->uid;

	request.payload.buf  = (void*)name;
	request.payload.size = strlen(name) + 1;

	send(&request);
	wait(request.to);

	struct msg response = {{0}};
	recv(&response);

	if (response.code == -1)
		return NULL;

	vfs_node_t* node = add_node(parent, name);
	node->uid  = response.code;
	node->type = response.args[0];

	return node;
}

vfs_node_t* get_node(vfs_node_t* parent, const char* path, bool virt)
{
	if (!parent && path[0] == '/')
	{
		parent = &root_node;
		path += 1;
	}

	if (!parent)
		return NULL;

	const char* path_end = strchr(path, '\0');
	if (path == path_end)
		return parent;

	while (path != path_end + 1)
	{
		const char* component = strchr(path, '/');
		if (!component)
			component = path_end;

		vfs_node_t* curr = parent->children;
		while (curr)
		{
			if (memcmp(curr->name, path, component - path) == 0)
				break;

			curr = curr->next;
		}

		if (!curr)
		{
			char curr_name[32] = {0};
			memcpy(curr_name, path, component - path);

			curr = request_node(parent, curr_name, virt);
		}

		parent = curr;
		path = component + 1;
	}

	return parent;
}

vfs_node_t* add_mount(const char* path, const char* name, ipcport_t owner)
{
	vfs_node_t* parent = get_node(NULL, path, true);
	if (!parent || parent->type != VFS_VIRTDIR)
		return NULL;

	vfs_node_t* mount = add_node(parent, name);
	mount->type = VFS_MOUNTPT;
	mount->owner = owner;

	return mount;
}

int open_node(vfs_node_t* node, pid_t target)
{
	file_table_t* table = &(file_tables[target]);
	if (table->count >= 8)
		return -1;

	int slot = -1;
	for (int i = 0; i != 8; ++i)
	{
		if (!(table->nodes[i]))
		{
			slot = i;
			break;
		}
	}

	if (slot < 0)
		return -1;

	table->nodes[slot] = node;
	table->count += 1;

	return slot;
}

int read_node(vfs_node_t* node, void* data, size_t size, size_t off)
{
	struct msg request = {{0}};
	request.to = node->owner;

	request.code = VFS_FSREAD;
	request.args[0] = node->uid;
	request.args[1] = size;
	request.args[2] = off;

	send(&request);
	wait(request.to);

	struct msg response = {{0}};
	response.payload.buf  = data;
	response.payload.size = size;

	recv(&response);
	return response.code;
}

int write_node(vfs_node_t* node, void* data, size_t size, size_t off)
{
	struct msg request = {{0}};
	request.to = node->owner;

	request.code = VFS_FSWRITE;
	request.args[0] = node->uid;
	request.args[1] = size;
	request.args[2] = off;

	request.payload.buf  = data;
	request.payload.size = size;

	send(&request);
	wait(request.to);

	struct msg response = {{0}};
	recv(&response);

	return response.code;
}

void handle_kernel(struct msg* notice)
{
	// Nothing yet.
}

void handle_procmgr(struct msg* notice)
{
	if (notice->code != PN_STATECH)
		return;

	pid_t pid = notice->args[0];
	memset(file_tables[pid], 0, sizeof(file_table_t));
}

void handle_request(struct msg* request, struct msg* response)
{
	ipcport_t target = request->from;
	msgdata_t action = request->code;
	char* buffer = (char*)(request->payload.buf);

	response->code = -1;
	switch (action)
	{
		case VFS_MOUNT:
		{
			char* path_end = strrchr(buffer, '/');
			*path_end = '\0';

			char* path = (path_end == buffer) ? "/" : buffer;
			char* name = path_end + 1;

			vfs_node_t* root = add_mount(path, name, target);

			response->code = (root) ? 0 : -1;
			break;
		}
		case VFS_OPEN:
		{
			vfs_node_t* node = get_node(NULL, buffer, false);
			if (node->type != VFS_FILE)
				return;

			int fd = open_node(node, IPORTPROC(target));
			
			response->code = fd;
			break;
		}
		case VFS_READ:
		{
			pid_t pid = IPORTPROC(target);
			int fd = request->args[0];
			size_t size = request->args[1];
			size_t off = request->args[2];

			vfs_node_t* node = file_tables[pid]->nodes[fd];
			if (!node)
				return;

			void* data = malloc(size);
			int res = read_node(node, data, size, off);

			response->code = res;
			if (res <= 0)
			{
				free(data);
				return;
			}

			response->payload.buf  = data;
			response->payload.size = res;
			break;
		}
		case VFS_WRITE:
		{
			pid_t pid = IPORTPROC(target);
			int fd = request->args[0];
			size_t size = request->args[1];
			size_t off = request->args[2];

			vfs_node_t* node = file_tables[pid]->nodes[fd];
			if (!node)
				return;

			int res = write_node(node, data, size, off);

			response->code = res;
			break;
		}
	}
}

int main()
{
	if (svcown(SVC_VFS) < 0)
		return 1;

	while ((procmgr = svcid(SVC_PROCMGR)) == 0);

	file_tables = malloc(sizeof(file_table_t) * 1024);
	memset(file_tables, 0, sizeof(file_table_t) * 1024);

	char buffer[256];
	while (true)
	{
		wait(IPORT_ANY);

		struct msg msg_in = {{0}};
		msg_in.payload.buf  = buffer;
		msg_in.payload.size = 256;

		if (recv(&msg_in) < 0)
		{
			drop(NULL);
			continue;
		}

		switch (msg_in.from)
		{
			case IPORT_KERNEL:
			{
				handle_kernel(&msg_in);
				break;
			}
			case procmgr:
			{
				handle_procmgr(&msg_in);
				break;
			}
			default:
			{
				struct msg msg_out = {{0}};
				handle_request(&msg_in, &msg_out);

				msg_out.to = msg_in.from;
				send(&msg_out);

				if (msg_out.payload.buf)
					free(msg_out.payload.buf);

				break;
			}
		}
	}

	free(file_tables);
	return 0;
}
