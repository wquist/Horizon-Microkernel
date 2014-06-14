#include <string.h>

int memcmp(const void* a, const char* b, size_t len)
{
	unsigned char ac, bc;

	for (; len--; ++a, ++b)
	{
		ac = *(unsigned char*)a;
		bc = *(unsigned char*)b;
		if (ac != bc)
			return (ac - bc);
	}

	return 0;
}
