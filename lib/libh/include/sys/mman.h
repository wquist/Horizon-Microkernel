#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#include <stddef.h>
#include <stdint.h>

void* vmap(void* dest, size_t size);
void* pmap(void* dest, uintptr_t src, size_t size);

long unmap(void* addr, size_t size);

#endif
