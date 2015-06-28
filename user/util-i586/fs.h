#pragma once

#include "../vfsd-all/fs.h"
#include "msg.h"

int fs_mount(ipcport_t fs, const char* path)
{
	struct msg req;
	msg_create(&req, fs, VFS_MOUNT);

	msg_attach_payload(&req, (void*)path, strlen(path)+1);

	send(&req);
	wait(req.to);

	struct msg res;
	recv(&res);

	return res.code;
}
