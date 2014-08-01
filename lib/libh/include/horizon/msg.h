#ifndef _HORIZON_MSG_H
#define _HORIZON_MSG_H

#include <features.h>
#include <horizon/types.h>

#define MDSTFMT(uid, knd) ((msgdst_t)(uid) | ((msgdst_t)(knd) << 16))

#define MSRCPID(src) ((pid_t)(((msgsrc_t)(src) >> 16) & 0xFFFF))
#define MSRCTID(src) ((tid_t)((msgsrc_t)(src) & 0xFFFF))

enum
{
	MTOPID = 0,
	MTOTID
};

struct msg
{
	union
	{
		msgdst_t to;
		msgsrc_t from;
	} NONAMELESSFIELD(info);

	msgarg_t code, arg;
	msgarg_t data;

	struct
	{
		void* buf;
		size_t size;
	} payload;
};

#endif
