#include <horizon/ipc.h>
#include <sys/svc.h>
#include <sys/sched.h>
#include <sys/msg.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>

#include "../util-i586/msg.h"
#include "../util-i586/fs.h"
#include "../vfsd-all/fs.h"
#include "fat.h"

ipcport_t filesystem;

ipcport_t device;
int disk;

int get_dev_addr(const char* path, int* fd)
{
	struct msg req;
	msg_create(&req, filesystem, VFS_DIRECT);

	msg_attach_payload(&req, (void*)path, strlen(path)+1);

	send(&req);
	wait(req.to);

	struct msg res;
	recv(&res);

	*fd = res.args[0];
	return res.code;
}

int main()
{
	while ((filesystem = svcid(SVC_VFS)) == 0);
	while ((device = get_dev_addr("/dev/ata", &disk)) == -1);

	fat_volume_t vol = {0};
	fat_init(device, disk, &vol);

	if (fs_mount(filesystem, "/usr") < 0)
		return 1;

	while (true)
	{
		struct msg req;
		if (msg_get_waiting(&req) < 0)
			continue;

		struct msg res;
		msg_create(&res, req.from, -1);

		switch (req.code)
		{
			case VFS_FSFIND:
			{
				fat_file_t* parent = (fat_file_t*)(req.args[0]);
				fat_file_t* file = malloc(sizeof(fat_file_t));

				size_t iter = 0;
				while ((iter = fat_enumerate(&vol, parent, iter, file)) != -1)
				{
					if (strcmp(file->name, req.payload.buf) == 0)
						break;
				}

				if (iter >= 0)
				{
					res.code = (uintptr_t)file;
					res.args[0] = file->type;
				}
				else
				{
					free(file);
				}

				send(&res);
				break;
			}
			case VFS_FSREAD:
			{
				fat_file_t* file = (fat_file_t*)(req.args[0]);
				size_t len = req.args[1];
				size_t off = req.args[2];

				uint8_t* buffer = malloc(len);
				int bytes = fat_read(&vol, file, off, len, buffer);

				res.code = bytes;
				if (bytes > 0)
					msg_attach_payload(&res, buffer, bytes);

				send(&res);
				free(buffer);

				break;
			}
			default:
			{
				send(&res);
				break;
			}
		}

		if (req.payload.size)
			free(req.payload.buf);
	}

	for (;;);
	return 0;
}
