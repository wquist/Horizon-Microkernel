#include <string.h>

char* strcpy(char* dst, const char* src)
{
	size_t i;
	for (i = 0; src[i]; ++i)
		dst[i] = src[i];

	dst[i] = 0;
	return dst;
}
