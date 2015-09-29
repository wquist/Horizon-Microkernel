#include <malloc.h>
#include <internal/malloc.h>
#include <sys/mman.h>
#include <limits.h>

static void* heap_base;
static void* heap_top;

void __malloc_initialize(void* base)
{
	heap_base = base;
	heap_top  = base;
}

void* malloc_heap_start()
{
	return heap_base;
}

void* malloc_heap_end()
{
	return heap_top;
}

void* malloc_heap_expand(size_t min_size)
{
	size_t size = (min_size + PAGE_SIZE-1) & ~(PAGE_SIZE-1);
	void* res = vmap(heap_top, size);
	if (!res)
		return NULL;

	struct malloc_header* next = heap_top;
	next->size = size;

	heap_top += size;
	return (void*)next;
}
