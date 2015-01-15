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

struct vfs_node;

struct vfs_root
{
	char name[32];
	ipcport_t owner;

	//

	struct vfs_root* next;
	struct vfs_node* nodes;
};

struct vfs_node
{
	char name[32];
	size_t uid;
	enum VFS_FTYPE type;

	//

	struct vfs_root* root;
	struct vfs_node* next;
	struct vfs_node* children;
};

static struct vfs_root* root_list = NULL;

struct vfs_root* add_root(const char* name, ipcport_t owner)
{
	struct vfs_root* new_root = malloc(sizeof(struct vfs_root));
	memset(new_root, 0, sizeof(struct vfs_root));

	strcpy(new_root->name, name);
	new_root->owner = owner;

	new_root->next = root_list;
	root_list = new_root;

	return new_root;
}

struct vfs_root* get_root(const char* path, const char** path_start)
{
	const char* first = strchr(path, ':');
	if (!first)
		return NULL;

	if (memcmp("//", first+1, 2) != 0)
		return NULL;

	*path_start = first+3;

	struct vfs_root* curr = root_list;
	while (curr)
	{
		if (memcmp(curr->name, path, first - path) == 0)
			return curr;

		curr = curr->next;
	}

	return NULL;
}

struct vfs_node* request_node(struct vfs_root* root, struct vfs_node* parent, const char* name)
{
	struct msg node_req = {{0}};
	node_req.to      = root->owner;
	node_req.code    = VFS_REQ_FIND;
	node_req.args[0] = (parent) ? parent->uid : 0;

	node_req.payload.buf  = (void*)name;
	node_req.payload.size = strlen(name)+1;

	send(&node_req);
	wait(root->owner);

	struct msg node_response = {{0}};
	recv(&node_response);

	if (node_response.code != VFS_REQ_FIND)
		return NULL;

	struct vfs_node* new_node = malloc(sizeof(struct vfs_node));
	memset(new_node, 0, sizeof(struct vfs_node));

	strcpy(new_node->name, name);
	new_node->uid  = node_response.args[0];
	new_node->type = node_response.args[1];

	if (parent)
	{
		new_node->next = parent;
		parent->children = new_node;
	}
	else
	{
		new_node->next = root->nodes;
		root->nodes = new_node;
	}

	return new_node;
}

struct vfs_node* get_node(struct vfs_root* root, const char* path)
{
	const char* path_end = strchr(path, '\0');

	struct vfs_node* level_node = root->nodes;
	struct vfs_node* parent_node = NULL;
	struct vfs_node* target_node;

	while (path != path_end+1)
	{
		const char* level_name = strchr(path, '/');
		if (!level_name)
			level_name = path_end;

		target_node = NULL;
		while (level_node)
		{
			if (memcmp(level_node->name, path, level_name - path) == 0)
			{
				target_node = level_node;
				break;
			}

			level_node = level_node->next;
		}

		if (!target_node)
		{
			char node_name[32] = {0};
			memcpy(node_name, path, level_name - path);

			target_node = request_node(root, parent_node, node_name);
			if (!target_node)
				return NULL;
		}

		if (target_node->type != VFS_DIR && level_name != path_end)
			return NULL;

		level_node  = target_node->children;
		parent_node = target_node;
		path = level_name+1;
	}

	return target_node;
}

int main()
{
	if (svcown(SVC_VFS) < 0)
		return 1;

	char buffer[256];
	while (true)
	{
		wait(IPORT_ANY);

		struct msg request = {{0}};
		request.payload.buf  = buffer;
		request.payload.size = 256;

		if (recv(&request) < 0)
		{
			drop(NULL);
			continue;
		}

		struct msg response = {{0}};
		response.to = request.from;
		switch (request.code)
		{
			case VFS_MOUNT:
			{
				add_root(buffer, request.from);
				response.code = 0;

				send(&response);
				break;
			}
			case VFS_OPEN:
			{
				const char* path;
				struct vfs_root* root = get_root(buffer, &path);

				struct vfs_node* node = get_node(root, path);
				if (node)
					response.code = (uintptr_t)node;
				else
					response.code = -1;

				send(&response);
				break;
			}
			case VFS_READ:
			{
				void* read_buffer = NULL;

				struct vfs_node* node = (struct vfs_node*)(request.args[0]);
				if (node->type == VFS_FILE)
				{
					size_t read_size = request.args[1];
					read_buffer = malloc(read_size);

					struct msg read_request = {{0}};
					read_request.to = node->root->owner;

					read_request.code = VFS_READ;
					read_request.args[0] = node->uid;
					read_request.args[1] = read_size;

					send(&read_request);
					wait(read_request.to);

					struct msg read_response = {{0}};
					read_response.payload.buf  = read_buffer;
					read_response.payload.size = read_size;

					recv(&read_response);

					response.code = read_response.code;
					if (response.code != -1)
					{
						response.payload.buf  = read_buffer;
						response.payload.size = read_size;
					}
				}
				else
				{
					response.code = -1;
				}

				send(&response);
				if (read_buffer)
					free(read_buffer);

				break;
			}
			case VFS_WRITE:
			{
				struct vfs_node* node = (struct vfs_node*)(request.args[0]);
				if (node->type == VFS_FILE)
				{
					size_t write_size = request.args[1];

					struct msg write_request = {{0}};
					write_request.to = node->root->owner;

					write_request.code = VFS_WRITE;
					write_request.args[0] = node->uid;
					write_request.args[1] = write_size;

					write_request.payload.buf  = buffer;
					write_request.payload.size = write_size;

					send(&write_request);
					wait(write_request.to);

					struct msg write_response = {{0}};
					recv(&write_response);

					response.code = write_response.code;
				}
				else
				{
					response.code = -1;
				}

				send(&response);
				break;
			}
			default:
			{
				response.code = -1;

				send(&response);
				break;
			}
		}
	}

	return 0;
}
