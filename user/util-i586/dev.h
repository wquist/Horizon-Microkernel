#pragma once

#include <sys/svc.h>
#include "msg.h"

static int dev_register(ipcport_t dm, const char* name)
{
	struct msg req;
	msg_create(&req, dm, 100);

	msg_attach_payload(&req, (void*)name, strlen(name)+1);

	send(&req);
	wait(req.to);

	struct msg res;
	recv(&res);

	return res.code;
}
