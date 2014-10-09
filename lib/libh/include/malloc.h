#ifndef _MALLOC_H
#define _MALLOC_H

#include <stddef.h>

#define MALLOC_ALIGNMENT (sizeof(size_t) * 2)

void malloc_initialize(void* head);

void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
void  free(void* ptr);

#endif
