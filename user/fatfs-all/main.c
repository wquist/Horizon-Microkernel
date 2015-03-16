#include <horizon/ipc.h>
#include <sys/sched.h>
#include <sys/svc.h>
#include <sys/msg.h>
#include <sys/proc.h>
#include <stdbool.h>
#include <string.h>

#include "fat.h"
#include "../vfsd-all/fs.h"

ipcport_t filesystem;
int screen;

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

	fat_volume_t vol = {0};
	fat_init(disk, &vol);

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
				//
			}
			case VFS_FSREAD:
			{
				//
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
