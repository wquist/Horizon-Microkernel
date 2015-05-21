#include <horizon/ipc.h>
#include <sys/sched.h>
#include <sys/svc.h>
#include <sys/msg.h>
#include <malloc.h>
#include <string.h>
#include <memory.h>
#include <stdbool.h>

#include "../vfsd-all/fs.h"

ipcport_t filesystem = 0;

int main()
{
	while ((filesystem = svcid(SVC_VFS)) == 0);

	struct msg mount_request = {{0}};
	mount_request.to = filesystem;

	mount_request.code = VFS_MOUNT;
	mount_request.payload.buf  = "/tmp";
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
				response.code = 1;
				response.args[0] = VFS_FILE;

				send(&response);
				break;
			}
			case VFS_FSREAD:
			{
				char* buf = malloc(request.args[1]);
				memset(buf, 'a', request.args[1]);

				response.code = request.args[1];
				response.payload.buf = buf;
				response.payload.size = request.args[1];

				send(&response);

				free(buf);
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
