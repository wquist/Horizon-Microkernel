#ifndef _HORIZON_IPC_H
#define _HORIZON_IPC_H

#include <horizon/types.h>

#define IPORT_KERNEL (1)
#define IPORT_ANY    (0xFFFFFFFF)

#define IPORTID(port) (((port) >> 6) & 0x3FFFF)

#endif
