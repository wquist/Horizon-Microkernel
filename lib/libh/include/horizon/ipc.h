#ifndef _HORIZON_IPC_H
#define _HORIZON_IPC_H

#include <horizon/types.h>

#define IDST_KERNEL (1)
#define IDST_ANY    (0xFFFF)

#define IDST_PROC(uid)   ((ipcdst_t)(uid) | ((ipcdst_t)(0) << 16))
#define IDST_THREAD(uid) ((ipcdst_t)(uid) | ((ipcdst_t)(1) << 16))

#endif
