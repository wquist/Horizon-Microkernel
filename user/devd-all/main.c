#include <horizon/ipc.h>
#include <sys/svc.h>
#include <sys/sched.h>
#include <sys/msg.h>
#include <stdbool.h>
#include <malloc.h>

#include "../vfsd-all/fs.h"
#include "../util-i586/msg.h"
#include "device.h"

int request_read(device_t* dev, size_t off, size_t size, void* buffer)
{
	struct msg req;
	msg_create(&req, dev->port, 0);

	msg_set_args(&req, 2, size, off);

	send(&req);
	wait(req.to);

	struct msg res;
	msg_attach_payload(&res, buffer, size);

	recv(&res);
	return res.code;
}

int request_write(device_t* dev, size_t off, size_t size, void* buffer)
{
	struct msg req;
	msg_create(&req, dev->port, 1);

	msg_set_args(&req, 2, size, off);
	msg_attach_payload(&req, buffer, size);

	send(&req);
	wait(req.to);

	struct msg res;
	recv(&res);

	return res.code;
}

int main()
{
	ipcport_t filesystem;
	while ((filesystem = svcid(SVC_VFS)) == 0);

	struct msg mnt_req;
	msg_create(&mnt_req, filesystem, VFS_MOUNT);

	msg_attach_payload(&mnt_req, "/dev", 5);

	send(&mnt_req);
	wait(mnt_req.to);

	struct msg mnt_res;
	recv(&mnt_res);

	if (mnt_res.code == -1)
		return 1;

	if (svcown(SVC_DEVMGR) < 0)
		return 1;

	while (true)
	{
		struct msg req;
		if (msg_get_waiting(&req) < 0)
			continue;

		switch (req.code)
		{
			struct msg res;
			msg_create(&res, req.from, -1);

			case 100:
			{
				device_add(req.payload.buf, req.from);

				res.code = 0;

				send(&res);
				break;
			}
			case VFS_FSFIND:
			{
				device_t* dev = device_find(req.payload.buf);
				if (!dev)
				{
					send(&res);
					break;
				}

				res.code = dev->uid;
				msg_set_args(&res, 1, 0);

				send(&res);
				break;
			}
			case VFS_FSREAD:
			{
				device_t* dev = device_get(req.args[0]);
				if (!dev)
				{
					send(&res);
					break;
				}

				size_t size = req.args[1];
				size_t off = req.args[2];

				void* buffer = malloc(size);
				int bytes = request_read(dev, off, size, buffer);

				res.code = bytes;
				if (bytes != -1)
					msg_attach_payload(&res, buffer, bytes);

				send(&res);

				free(buffer);
				break;
			}
			case VFS_FSWRITE:
			{
				device_t* dev = device_get(req.args[0]);
				if (!dev)
				{
					send(&res);
					break;
				}

				size_t size = req.args[1];
				size_t off = req.args[2];

				int bytes = request_write(dev, off, size, req.payload.buf);
				res.code = bytes;

				send(&res);
				break;
			}
			default:
			{
				send(&res);
				break;
			}
		}

		if (req.payload.buf)
			free(req.payload.buf);
	}
}
