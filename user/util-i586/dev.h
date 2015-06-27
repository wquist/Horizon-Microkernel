#pragma once

#include <sys/svc.h>
#include "msg.h"

static int dev_register(const char* name)
{
	ipcport_t devmgr;
	while ((devmgr = svcid(SVC_DEVMGR)) == 0);

	struct msg req;
	msg_create(&req, devmgr, 100);

	msg_attach_payload(&req, (void*)name, strlen(name));

	send(&req);
	wait(req.to);

	struct msg res;
	recv(&res);

	return res.code;
}
