#pragma once

#include <horizon/types.h>
#include <stdint.h>

typedef struct device device_t;
struct device
{
	char name[32];
	size_t uid;
	ipcport_t port;

	device_t* next;
};

void device_add(const char* name, ipcport_t port);

device_t* device_get(size_t uid);
device_t* device_find(const char* name);
