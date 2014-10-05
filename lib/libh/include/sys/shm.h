#ifndef _SYS_SHM_H
#define _SYS_SHM_H

#include <horizon/shm.h>
#include <horizon/types.h>
#include <stdint.h>

long grant(struct shm* info, uintptr_t dest);
shmid_t share(struct shm* info);
size_t accept(shmid_t sid, void* dest, size_t size);

#endif
