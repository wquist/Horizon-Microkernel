#include <horizon/ipc.h>
#include <sys/svc.h>
#include <sys/sched.h>
#include <sys/msg.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>

#include "node.h"
#include "fs.h"
#include "../util-i586/msg.h"

typedef struct file_table file_table_t;
struct file_table
{
	size_t count;
	node_t* nodes[8];
};

static node_t root_node = {{0}};
static file_table_t file_tables[1024] = {{0}};

node_t* mount_add(const char* path, const char* name, ipcport_t owner)
{
	node_t* parent = node_find(&root_node, path+1, true);
	if (!parent || parent->type != NODE_VIRT)
		return NULL;

	node_t* node = node_add(parent, name);
	node->type = NODE_MOUNT;
	node->owner = owner;

	return node;
}

int file_open(node_t* node, pid_t target)
{
	file_table_t* table = &(file_tables[target]);
	if (table->count >= 8)
		return -1;

	int slot = -1;
	for (size_t i = 0; i != 8; ++i)
	{
		if (table->nodes[i])
			continue;

		slot = i;
		break;
	}

	table->nodes[slot] = node;
	table->count += 1;

	return slot;
}

int file_read(node_t* node, void* data, size_t size, size_t off)
{
	struct msg req;
	msg_create(&req, node->owner, VFS_FSREAD);

	msg_set_args(&req, 3, node->uid, size, off);

	send(&req);
	wait(req.to);

	struct msg res;
	msg_attach_payload(&res, data, size);

	recv(&res);
	return res.code;
}

int file_write(node_t* node, void* data, size_t size, size_t off)
{
	struct msg req;
	msg_create(&req, node->owner, VFS_FSWRITE);

	msg_set_args(&req, 3, node->uid, size, off);
	msg_attach_payload(&req, data, size);

	send(&req);
	wait(req.to);

	struct msg res;
	recv(&res);

	return res.code;
}

void handle_request(struct msg* req, struct msg* res)
{
	ipcport_t target = req->from;
	char* buffer = (char*)(req->payload.buf);

	msg_create(res, target, -1);
	switch (req->code)
	{
		case VFS_MOUNT:
		{
			char* path_end = strrchr(buffer, '/');
			*path_end = '\0';

			char* path = (path_end == buffer) ? "/" : buffer;
			char* name = path_end + 1;

			node_t* root = mount_add(path, name, target);
			root->uid = 0;

			res->code = (root) ? 0 : -1;
			break;
		}
		case VFS_OPEN:
		{
			node_t* node = node_find(&root_node, buffer+1, false);
			if (!node || node->type != NODE_FILE)
				return;

			int fd = file_open(node, IPORTPROC(target));

			res->code = fd;
			break;
		}
		case VFS_DIRECT:
		{
			node_t* node = node_find(&root_node, buffer+1, false);
			if (!node || node->type != NODE_FILE)
				return;

			msg_set_args(res, 1, node->uid);

			res->code = node->owner;
			break;
		}
		case VFS_READ:
		{
			pid_t pid = IPORTPROC(target);
			int fd = req->args[0];
			size_t size = req->args[1];
			size_t off = req->args[2];

			node_t* node = file_tables[pid].nodes[fd];
			if (!node)
				return;

			void* data = malloc(size);
			int bytes = file_read(node, data, size, off);
			if (bytes <= 0)
			{
				free(data);
				return;
			}

			msg_attach_payload(res, data, bytes);

			res->code = bytes;
			break;
		}
		case VFS_WRITE:
		{
			pid_t pid = IPORTPROC(target);
			int fd = req->args[0];
			size_t size = req->args[1];
			size_t off = req->args[2];

			node_t* node = file_tables[pid].nodes[fd];
			if (!node)
				return;

			int bytes = file_write(node, buffer, size, off);

			res->code = bytes;
			break;
		}
	}
}

int main()
{
	if (svcown(SVC_VFS) < 0)
		return 1;

	strcpy(root_node.name, "ROOT");
	root_node.type = NODE_VIRT;

	while (true)
	{
		struct msg req;
		if (msg_get_waiting(&req) < 0)
			continue;

		switch (req.from)
		{
			//
			default:
			{
				struct msg res;
				handle_request(&req, &res);

				send(&res);
				if (res.payload.size)
					free(res.payload.buf);

				break;
			}
		}

		if (req.payload.size)
			free(req.payload.buf);
	}

	for (;;);
	return 0;
}
