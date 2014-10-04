#ifndef _SYS_SHM_H
#define _SYS_SHM_H

#include <horizon/shm.h>
#include <horizon/types.h>
#include <stdint.h>

long grant(struct shm* info, uintptr_t dest);
long share(struct shm* info);
long accept(shmid_t sid, void* dest, size_t size);

#endif
