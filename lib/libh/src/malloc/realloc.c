#include <malloc.h>
#include <internal/malloc.h>
#include <memory.h>

void* realloc(void* ptr, size_t size)
{
	struct malloc_header* header = ptr - sizeof(struct malloc_header);

	size_t old_size = header->size & ~MALLOC_USED;
	size_t new_size = MALLOC_ALIGN(size + sizeof(struct malloc_header));
	if (new_size <= old_size)
		return ptr;

	void* new_ptr = malloc(size);
	memcpy(new_ptr, ptr, old_size - sizeof(struct malloc_header));
	free(ptr);

	return new_ptr;
}
