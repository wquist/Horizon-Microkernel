#ifndef _HORIZON_SHM_H
#define _HORIZON_SHM_H

#include <horizon/types.h>

#define SPROT_READ  (0 << 0)
#define SPROT_WRITE (1 << 0)

#define SOPT_FORCE (1 << 0)

struct shm
{
	ipcchan_t to;

	void*  addr;
	size_t size;

	uint8_t prot;
};

#endif
