#include <horizon/ipc.h>
#include <sys/sched.h>
#include <sys/svc.h>
#include <sys/msg.h>
#include <sys/proc.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>

#include "idectl.h"
#include "../vfsd-all/fs.h"

ipcport_t devmgr;

bool register_device(char* name)
{
	struct msg request = {{0}};
	request.to = devmgr;
	request.code = 100;
	request.payload.buf = name;
	request.payload.size = strlen(name);

	send(&request);
	wait(devmgr);

	struct msg response = {{0}};
	recv(&response);
	if (response.code != 0)
		return false;

	return true;
}

int main()
{
	while ((devmgr = svcid(SVC_DEVMGR)) == 0);

	ide_controller_t ctl = { .base = 0x1F0 };
	idectl_identify(&ctl, IDE_MASTER);

	if (!(ctl.devices[IDE_MASTER].present))
		return 1;
	if (!register_device("ata"))
		return 1;

	char buffer[512];
	while (true)
	{
		wait(IPORT_ANY);

		struct msg request = {{0}};
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
				size_t off = request.args[1] / 512;
				idectl_block_io(&ctl, IDE_MASTER, IDE_READ, off, 1, buffer);

				response.code = 512;
				response.payload.buf  = buffer;
				response.payload.size = 512;

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
