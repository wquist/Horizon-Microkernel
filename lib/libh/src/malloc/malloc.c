#include <malloc.h>
#include <internal/malloc.h>

static void* find_fit(size_t size);

void* malloc(size_t size)
{
	size_t block_size = MALLOC_ALIGN(size + sizeof(struct malloc_header));

	struct malloc_header* header = find_fit(block_size);
	if (!header)
	{
		header = malloc_heap_expand(block_size);
		if (!header)
			return NULL;
	}

	size_t remaining = header->size - block_size;
	if (remaining >= MALLOC_ALIGNMENT*2)
	{
		char* next_ptr = (char*)header + block_size;
		struct malloc_header* next = (struct malloc_header*)next_ptr;

		next->size = remaining;
		header->size = block_size;
	}

	header->size |= MALLOC_USED;
	return (char*)header + sizeof(struct malloc_header);
}

void* find_fit(size_t size)
{
	struct malloc_header* header = malloc_heap_start();
	while ((void*)header < malloc_heap_end())
	{
		if (header->size >= size && !(header->size & MALLOC_USED))
			return header;

		char* header_ptr = (char*)header + (header->size & ~MALLOC_USED);
		header = (struct malloc_header*)header_ptr;
	}

	return NULL;
}
