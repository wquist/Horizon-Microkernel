#ifndef _SYS_SVC_H
#define _SYS_SVC_H

#include <horizon/svc.h>
#include <horizon/types.h>

long svcown(svc_t sid);
ipcport_t svcid(svc_t sid);

#endif
