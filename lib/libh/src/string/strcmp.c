#include <string.h>

int strcmp(const char* a, const char* b)
{
	size_t i;
	for (i = 0; a[i] != 0 && a[i] == b[i]; ++i);

	if (a[i] >  b[i]) return 1;
	if (a[i] == b[i]) return 0;
	return -1;
}
