#include <string.h>

char* strchr(const char* s, int ch)
{
	while (*s && *s != ch) ++s;

	if (*s == ch) return (char*)s;
	return NULL;
}
