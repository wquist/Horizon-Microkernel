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

typedef struct open_file open_file_t;
struct open_file
{
	fat_file_t file_info;
	size_t ref_count;

	open_file_t* next;
};

ipcport_t filesystem;

open_file_t* open_files = NULL;

int open(const char* path)
{
	struct msg request = {{0}};
	request.to = filesystem;

	request.code = VFS_OPEN;
	request.payload.buf  = (void*)path;
	request.payload.size = strlen(path)+1;

	send(&request);
	wait(filesystem);

	struct msg response = {{0}};
	recv(&response);

	return response.code;
}

int main()
{
	while ((filesystem = svcid(SVC_VFS)) == 0);

	int disk;
	while ((disk = open("/dev/ata")) == -1);

	fat_volume_t vol = {0};
	fat_init(disk, &vol);

	struct msg mount_request = {{0}};
	mount_request.to = filesystem;

	mount_request.code = VFS_MOUNT;
	mount_request.payload.buf  = "/home";
	mount_request.payload.size = 5;

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
				open_file_t* parent = (open_file_t*)(request.args[0]);
				open_file_t* found = malloc(sizeof(open_file_t));
				memset(found, 0, sizeof(open_file_t));

				size_t iter = 0;
				while (found->ref_count != 1)
				{
					iter = fat_enumerate(&vol, &(parent->file_info), iter, &(found->file_info));
					if (iter == -1)
						break;

					if (strcmp(buffer, found->file_info.name) == 0)
						found->ref_count = 1;
				}

				if (found->ref_count == 1)
				{
					found->next = open_files;
					open_files = found;

					response.code = (uintptr_t)found;
					response.args[0] = found->file_info.type;
				}
				else
				{
					free(found);
					response.code = -1;
				}

				send(&response);
				break;
			}
			case VFS_FSREAD:
			{
				open_file_t* target = (open_file_t*)(request.args[0]);
				size_t len = request.args[1];

				uint8_t* dest = malloc(len);
				size_t res = fat_read(&vol, &(target->file_info), request.args[2], len, dest);

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
