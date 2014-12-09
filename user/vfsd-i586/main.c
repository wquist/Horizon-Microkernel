#include <horizon/types.h>
#include <horizon/ipc.h>
#include <sys/msg.h>
#include <stddef.h>
#include <string.h>
#include <malloc.h>

#include "fs.h"

typedef enum FTYPE ftype_t;
enum FTYPE
{
	FTYPE_FILE,
	FTYPE_DIR,

	FTYPE_MOUNTPT
};

struct inode
{
	char name[64];
	ftype_t type;

	size_t uid;
	ipcport_t driver;

	struct inode* next;
	struct inode* children;
};

struct ftable
{
	pid_t proc;

	struct inode* files[16];
	size_t next_free;
};

static struct inode root = {0};
static struct ftable* tables = NULL;

struct inode* find_node(struct inode* start, const char* path)
{
	char* curr = path;
	struct inode* node = start;

	char* next;
	do
	{
		if (node->type != FTYPE_DIR && node->type != FTYPE_MOUNTPT)
			return NULL;

		next = strchr(curr, '/');

		static char component[64];
		if (next)
		{
			memcpy(component, curr, next-curr);
			component[next-curr] = '\0';
		}
		else
		{
			strcpy(component, curr);
		}

		struct inode* next_node = node->children;
		struct inode* tmp = NULL;

		while (next_node && !tmp)
		{
			if (strcmp(component, next_node->name) == 0)
				tmp = next_node;
			else
				next_node = next_node->next;
		}

		if (!tmp)
		{
			struct inode* child = malloc(sizeof(struct inode));
			memset(child, 0, sizeof(struct inode));
			strcpy(child->name, component);

			if (node->type == FTYPE_MOUNTPT)
			{
				child->type = FTYPE_MOUNTPT;
			}
			else
			{
				struct msg node_request = {{0}};
				node_request.to = node->driver;
				node_request.code = VFS_DRQ_FINDDIR;

				node_request.data[0] = node->uid;
				node_request.payload.buf  = component;
				node_request.payload.size = strlen(component);

				send(&node_request);
				wait(node->driver);

				struct msg node_response = {{0}};
				recv(&node_response);

				if (node_response.code == -1)
				{
					free(child);
					return NULL;
				}

				child->type = (node_response.data[1]) ? FTYPE_DIR : FTYPE_FILE;

				child->uid = node_response.data[0];
				child->driver = node->driver;
			}

			child->next = node->children;
			node->children = child;

			tmp = child;
		}
		
		node = tmp;
		curr = next+1;
	} while (next);

	return node;
}

int main()
{
	root.name = "/";
	root.type = FTYPE_MOUNTPT;

	return 0;
}
