#include <string.h>

void* memmove(void* dst, const void* src, size_t len)
{
	unsigned char* pd = dst;
	const unsigned char* ps = src;

	// FIXME: This comparison is not Standard-compliant
	if (ps < pd)
	{
		for (pd += len, ps += len; len--;)
			*--pd = *--ps;
	}
	else
	{
		while (len--)
			*pd++ = *ps++;
	}

	return dst;
}
