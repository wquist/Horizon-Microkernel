#ifndef _HORIZON_IPC_H
#define _HORIZON_IPC_H

#include <horizon/types.h>

#define IDST_ANY    (1)
#define IDST_KERNEL (0xFFFF)

#define IDST_PROC(uid)   ((msgdst_t)(uid) | ((msgdst_t)(0) << 16))
#define IDST_THREAD(uid) ((msgdst_t)(uid) | ((msgdst_t)(1) << 16))

#endif
