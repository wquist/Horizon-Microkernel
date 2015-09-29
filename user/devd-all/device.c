#include "device.h"
#include <string.h>
#include <malloc.h>

static device_t* device_head = NULL;
static size_t last_id = 1;

void device_add(const char* name, ipcport_t port)
{
	device_t* dev = malloc(sizeof(device_t));
	strcpy(dev->name, name);

	dev->uid = last_id++;
	dev->port = port;

	dev->next = device_head;
	device_head = dev;
}

device_t* device_get(size_t uid)
{
	device_t* curr = device_head;
	while (curr)
	{
		if (curr->uid == uid)
			return curr;

		curr = curr->next;
	}

	return NULL;
}

device_t* device_find(const char* name)
{
	device_t* curr = device_head;
	while (curr)
	{
		if (strcmp(curr->name, name) == 0)
			return curr;

		curr = curr->next;
	}

	return NULL;
}
