#include <string.h>

char* strncpy(char* dst, const char* src, size_t n)
{
	char* s = dst;

	while (n && src)
	{
		*s++ = *src++;
		--n;
	}

	if (n) *s = 0;
	return dst;
}
