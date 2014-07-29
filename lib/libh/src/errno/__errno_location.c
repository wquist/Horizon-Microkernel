#include <errno.h>

int* __errno_location()
{
	static int e;
	return &e;
}
