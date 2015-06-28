#include "node.h"
#include <sys/sched.h>
#include <sys/msg.h>
#include <string.h>
#include <malloc.h>

#include "fs.h"
#include "../util-i586/msg.h"

node_t* node_add(node_t* parent, const char* name)
{
	node_t* node = malloc(sizeof(node_t));
	memset(node, 0, sizeof(node_t));

	strcpy(node->name, name);
	node->owner = parent->owner;

	node->next = parent->children;
	parent->children = node;

	return node;
}

node_t* node_find(node_t* parent, const char* path, bool virt)
{
	if (*path == '\0')
		return parent;

	const char* path_end = strchr(path, '\0');
	while (parent && path != path_end+1)
	{
		const char* component = strchr(path, '/') ?: path_end;
		size_t complen = component - path;

		node_t* curr = parent->children;
		while (curr)
		{
			if (memcmp(curr->name, path, complen) == 0)
				break;

			curr = curr->next;
		}

		if (!curr)
		{
			char name[32] = {0};
			memcpy(name, path, complen);

			curr = node_request(parent, name, virt);
		}

		parent = curr;
		path = component + 1;
	}

	return parent;
}

node_t* node_request(node_t* parent, const char* name, bool virt)
{
	if (virt && parent->type == NODE_VIRT)
	{
		node_t* node = node_add(parent, name);
		node->type = NODE_VIRT;

		return node;
	}

	if (!(parent->owner))
		return NULL;

	struct msg req;
	msg_create(&req, parent->owner, VFS_FSFIND);

	msg_set_args(&req, 1, parent->uid);
	msg_attach_payload(&req, (void*)name, strlen(name)+1);

	send(&req);
	wait(req.to);

	struct msg res;
	recv(&res);

	if (res.code == -1)
		return NULL;

	node_t* node = node_add(parent, name);
	node->uid = res.code;
	node->type = (res.args[0] == 0) ? NODE_FILE : NODE_DIR;

	return node;
}
