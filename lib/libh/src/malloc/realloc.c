#include <malloc.h>
#include <internal/malloc.h>
#include <memory.h>

__hdecl void* realloc(void* ptr, size_t size)
{
	char* header_ptr = (char*)ptr - sizeof(struct malloc_header);
	struct malloc_header* header = (struct malloc_header*)header_ptr;

	size_t old_size = header->size & ~MALLOC_USED;
	size_t new_size = __malloc_align(size + sizeof(struct malloc_header));
	if (new_size <= old_size)
		return ptr;

	void* new_ptr = malloc(size);
	memcpy(new_ptr, ptr, old_size - sizeof(struct malloc_header));
	free(ptr);

	return new_ptr;
}
