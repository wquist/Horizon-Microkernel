#include <string.h>

void* memset(void* ptr, int ch, size_t len)
{
	unsigned char* s = ptr;
	for (; len; --len, ++s) 
		*s = ch;

	return ptr;
}
