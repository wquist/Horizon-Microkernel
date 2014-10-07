#ifndef _HORIZON_SVC_H
#define _HORIZON_SVC_H

#include <bits/svc.h>

typedef enum SVC svc_t;
enum SVC
{
	SVC_TERM = SVC_IMAX,
	SVC_KYBD,
	SVC_PGER,
	SVC_VFS,

	SVCMAX
};

#endif
