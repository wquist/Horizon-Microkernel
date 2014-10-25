#include <malloc.h>
#include <internal/malloc.h>

__hdecl void free(void* ptr)
{
	char* header_ptr = (char*)ptr - sizeof(struct malloc_header);
	struct malloc_header* header = (struct malloc_header*)header_ptr;
	
	header->size &= ~MALLOC_USED;

	char* next_ptr = (char*)header + header->size;
	struct malloc_header* next = (struct malloc_header*)next_ptr;

	if ((void*)next < malloc_heap_end() && !(next->size & MALLOC_USED))
		header->size += next->size;
}
