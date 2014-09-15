#ifndef _HORIZON_MSG_H
#define _HORIZON_MSG_H

#include <features.h>
#include <horizon/types.h>

struct msg
{
	union
	{
		ipcport_t to;
		ipcport_t from;
	};

	msgdata_t code, arg;

	struct
	{
		void* buf;
		size_t size;
	} payload;
};

#endif
