#include <string.h>

char* strrchr(const char* s, int ch)
{
	size_t i = strlen(s);
	while (i && s[i] != ch) --i;

	if (s[i] == ch) return (char*)(s + i);
	return NULL;
}
