#include <string.h>

size_t strlen(const char* s)
{
	size_t ret = 0;
	while (s[ret]) ++ret;
	return ret;
}
