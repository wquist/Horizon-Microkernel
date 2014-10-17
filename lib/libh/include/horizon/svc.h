#ifndef _HORIZON_SVC_H
#define _HORIZON_SVC_H

#include <bits/svc.h>

typedef enum SVC svc_t;
enum SVC
{
	SVC_PAGER = SVC_IMAX,
	SVC_PROCMGR,
	SVC_DEVMGR,
	SVC_VFS,

	SVCMAX
};

#endif
