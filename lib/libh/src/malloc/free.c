#include <malloc.h>
#include <internal/malloc.h>

void free(void* ptr)
{
	struct malloc_header* header = ptr - sizeof(struct malloc_header);
	header->size &= ~MALLOC_USED;

	struct malloc_header* next = (void*)header + header->size;
	if ((void*)next < malloc_heap_end() && !(next->size & MALLOC_USED))
		header->size += next->size;
}
