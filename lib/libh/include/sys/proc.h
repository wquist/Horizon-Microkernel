#ifndef _SYS_PROC_H
#define _SYS_PROC_H

#include <horizon/proc.h>
#include <horizon/types.h>
#include <stdint.h>

pid_t spawn();
long launch(pid_t pid, uintptr_t entry);
tid_t dispatch(void* entry, void* stack);

long release(tid_t tid);
long kill(pid_t pid);

#endif
