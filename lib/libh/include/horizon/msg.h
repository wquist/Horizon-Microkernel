#ifndef _HORIZON_MSG_H
#define _HORIZON_MSG_H

#include <features.h>
#include <horizon/types.h>

struct msg
{
	union
	{
		ipcchan_t to;
		struct
		{
			ipcchan_t channel;
			ipcsrc_t  source;
		} from;
	};

	msgarg_t code, arg;

	struct
	{
		void* buf;
		size_t size;
	} payload;
};

#endif
