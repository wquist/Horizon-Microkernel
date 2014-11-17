#include <horizon/types.h>
#include <horizon/ipc.h>
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
	//ipcport_t driver;

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

struct inode* find_node(const char* path)
{
	size_t len = strlen(path);
	if (len == 1)
		return (*path == '/') ? &root : NULL;

	char* curr = &path[1];
	struct inode* node = &root;

	char* next;
	do
	{
		next = strchr(curr, '/');

		char component[64];
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
		node = NULL;

		while (next_node && !node)
		{
			if (strcmp(component, next_node->name) == 0)
				node = next_node;
			else
				next_node = next_node->next;
		}

		if (!node)
			return NULL;

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
