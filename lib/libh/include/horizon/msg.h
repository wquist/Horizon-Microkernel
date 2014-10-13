#ifndef _HORIZON_MSG_H
#define _HORIZON_MSG_H

#include <features.h>
#include <horizon/types.h>

#define MSG_ARGC 3

struct msg
{
	union
	{
		ipcport_t to;
		ipcport_t from;
	};

	msgdata_t code;
	msgdata_t args[MSG_ARGC];

	struct
	{
		void* buf;
		size_t size;
	} payload;
};

#endif
