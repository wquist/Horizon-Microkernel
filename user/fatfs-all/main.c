#include <horizon/ipc.h>
#include <sys/sched.h>
#include <sys/svc.h>
#include <sys/msg.h>
#include <sys/proc.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>

#include "fat.h"
#include "../vfsd-all/fs.h"

ipcport_t filesystem;
ipcport_t device; int disk;

int open_device(const char* path)
{
	struct msg request = {{0}};
	request.to = filesystem;

	request.code = VFS_DOPEN;
	request.payload.buf  = (void*)path;
	request.payload.size = strlen(path)+1;

	send(&request);
	wait(filesystem);

	struct msg response = {{0}};
	recv(&response);

	device = response.code;
	disk = response.args[0];

	return response.code;
}

int main()
{
	while ((filesystem = svcid(SVC_VFS)) == 0);

	while (open_device("/dev/ata") == -1);

	fat_volume_t vol = {0};
	fat_init(device, disk, &vol);

	struct msg mount_request = {{0}};
	mount_request.to = filesystem;

	mount_request.code = VFS_MOUNT;
	mount_request.payload.buf  = "/home";
	mount_request.payload.size = 6;

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
			case VFS_FSFIND:
			{
				fat_file_t* parent = (fat_file_t*)(request.args[0]);
				fat_file_t* file = malloc(sizeof(fat_file_t));

				size_t iter = 0; bool found = false;
				while (!found)
				{
					memset(file, 0, sizeof(fat_file_t));

					iter = fat_enumerate(&vol, parent, iter, file);
					if (iter == -1)
						break;

					if (strcmp(file->name, buffer) == 0)
						found = true;
				}

				if (found)
				{
					response.code = (uintptr_t)file;
					response.args[0] = file->type;
				}
				else
				{
					free(file);
					response.code = -1;
				}

				send(&response);
				break;
			}
			case VFS_FSREAD:
			{
				fat_file_t* target = (fat_file_t*)(request.args[0]);
				size_t len = request.args[1];

				uint8_t* dest = malloc(len);
				size_t res = fat_read(&vol, target, request.args[2], len, dest);

				response.code = res;
				response.payload.buf  = dest;
				response.payload.size = res;

				send(&response);
				free(dest);
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
