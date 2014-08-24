#ifndef _HORIZON_MSG_H
#define _HORIZON_MSG_H

#include <features.h>
#include <horizon/types.h>

#define MSRC_PID(src) ((pid_t)(((msgsrc_t)(src) >> 16) & 0xFFFF))
#define MSRC_TID(src) ((tid_t)((msgsrc_t)(src) & 0xFFFF))

struct msg
{
	union
	{
		ipcdst_t to;
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
