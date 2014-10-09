#ifndef _INTERNAL_MALLOC_H
#define _INTERNAL_MALLOC_H

#include <malloc.h>

#define MALLOC_USED 1UL
#define MALLOC_ALIGN(x) (((x) + MALLOC_ALIGNMENT-1) & ~(MALLOC_ALIGNMENT-1))

struct malloc_header
{
	size_t size;
	struct malloc_header* prev;
};

void* malloc_heap_start();
void* malloc_heap_end();

void* malloc_heap_expand(size_t min_size);

#endif
