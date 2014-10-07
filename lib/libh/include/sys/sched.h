#ifndef _SYS_SCHED_H
#define _SYS_SCHED_H

#include <horizon/proc.h>
#include <horizon/types.h>

long yield();
long wait(ipcport_t sender);

#endif
