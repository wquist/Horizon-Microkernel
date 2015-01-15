// main.c

#include <horizon/ipc.h>
#include <sys/sched.h>
#include <sys/svc.h>
#include <sys/msg.h>
#include <malloc.h>
#include <string.h>
#include <memory.h>
#include <stdbool.h>

// :(
#include "../vfsd-i586/fs.h"

struct device
{
	char name[64];
	uint32_t uid;
	ipcport_t port;

	struct device* next;
};

ipcport_t filesystem = 0;
struct device* device_head = NULL;

uint32_t last_id = 0;

struct device* find_device(uint32_t uid)
{
	struct device* curr = device_head;
	while (curr != NULL)
	{
		if (curr->uid == uid)
			return curr;

		curr = curr->next;
	}

	return NULL;
}

void add_device(char* name, ipcport_t port)
{
	struct device* dev = malloc(sizeof(struct device));
	strcpy(dev->name, name);

	dev->uid  = last_id++;
	dev->port = port;

	dev->next = device_head;
	device_head = dev;
}

int main()
{
	if (svcown(SVC_DEVMGR) < 0)
		return 1;

	while ((filesystem = svcid(SVC_VFS)) == 0);

	struct msg mount_request = {{0}};
	mount_request.to = filesystem;

	mount_request.code = VFS_MOUNT;
	mount_request.payload.buf  = "dev";
	mount_request.payload.size = 4;

	send(&mount_request);
	wait(filesystem);

	struct msg mount_response = {{0}};
	recv(&mount_response);
	if (mount_response.code == -1)
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
			case 1:
			{
				add_device(buffer, request.from);

				response.code = 0;

				send(&response);
				break;
			}
			case VFS_READ:
			{
				void* read_buffer = NULL;

				struct device* dev = find_device(request.args[0]);
				if (dev)
				{
					size_t read_size = request.args[1];
					read_buffer = malloc(read_size);

					struct msg read_request = {{0}};
					read_request.to = dev->port;

					read_request.code = 1;
					read_request.args[0] = read_size;

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
				struct device* dev = find_device(request.args[0]);
				if (dev)
				{
					size_t write_size = request.args[1];

					struct msg write_request = {{0}};
					write_request.to = node->root->owner;

					write_request.code = 0;
					write_request.args[0] = write_size;

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
