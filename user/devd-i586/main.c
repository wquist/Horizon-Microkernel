// main.c

#include <horizon/ipc.h>
#include <sys/sched.h>
#include <sys/svc.h>
#include <sys/msg.h>
#include <malloc.h>
#include <string.h>
#include <memory.h>
#include <stdbool.h>

struct device
{
	char name[64];
	ipcport_t port;

	struct device* next;
};

struct device* device_head = NULL;

struct device* find_device(char* name)
{
	struct device* curr = device_head;
	while (curr != NULL)
	{
		if (strcmp(curr->name, name) == 0)
			return curr;

		curr = curr->next;
	}

	return NULL;
}

void add_device(char* name, ipcport_t port)
{
	struct device* dev = malloc(sizeof(struct device));
	strcpy(dev->name, name);
	dev->port = port;

	dev->next = device_head;
	device_head = dev;
}

int main()
{
	if (svcown(SVC_DEVMGR) < 0)
		return 1;

	char buffer[64];
	while (true)
	{
		wait(IPORT_ANY);

		struct msg request = {{0}};
		request.payload.buf  = buffer;
		request.payload.size = 64;

		if (recv(&request) < 0)
		{
			drop(NULL);
			continue;
		}

		struct msg response = {{0}};
		response.to = request.from;
		switch (request.code)
		{
			case 0:
			{
				struct device* dev = find_device(buffer);
				if (!dev)
					response.code = 0;
				else
					response.code = dev->port;

				send(&response);
				break;
			}
			case 1:
			{
				add_device(buffer, request.from);

				response.code = 0;

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
