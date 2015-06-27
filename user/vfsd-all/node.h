#pragma once

#include <horizon/ipc.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum NODE_TYPE node_type_t;
enum NODE_TYPE
{
	NODE_FILE,
	NODE_DIR,
	NODE_VIRT,
	NODE_MOUNT
};

typedef struct node node_t;
struct node
{
	char name[32];
	ipcport_t owner;

	size_t uid;
	node_type_t type;

	node_t* next;
	node_t* children;
};

node_t* node_add(node_t* parent, const char* name);

node_t* node_find(node_t* parent, const char* path, bool virt);
node_t* node_request(node_t* parent, const char* name, bool virt);
