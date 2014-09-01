#ifndef _HORIZON_IPC_H
#define _HORIZON_IPC_H

#include <horizon/types.h>

#define ICHAN_KERNEL (1)
#define ICHAN_ANY    (0xFFFF)

#define ICHAN_PROC(uid)   ((ipcchan_t)(uid) | ((ipcchan_t)(0) << 16))
#define ICHAN_THREAD(uid) ((ipcchan_t)(uid) | ((ipcchan_t)(1) << 16))

#define ICHANID(chan) (chan & 0xFFFF)

#endif
