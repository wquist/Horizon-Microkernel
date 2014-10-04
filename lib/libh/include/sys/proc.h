#ifndef _SYS_PROC_H
#define _SYS_PROC_H

#include <horizon/types.h>
#include <stdint.h>

long spawn();
long launch(pid_t pid, uintptr_t entry);
long dispatch(void* entry, void* stack);

long release(tid_t tid);
long kill(pid_t pid);

#endif
