#ifndef _HORIZON_IPC_H
#define _HORIZON_IPC_H

#include <horizon/types.h>

#define IPORT_KERNEL (1)
#define IPORT_ANY    (0)

#define IPORT_LOCAL(tid) ((tid) << 18)
#define IPORT_GLOBL(pid) (pid)

#define IPORTID(port) ((port) & 0x3FFFF)

#endif
