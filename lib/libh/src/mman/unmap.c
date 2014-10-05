#include <internal/syscall.h>
#include <internal/errno.h>
#include <sys/mman.h>
#include <errno.h>

long unmap(void* addr, size_t size)
{
	long res = syscall(SYS_unmap, addr, size);
	if (__iserr(res))
	{
		errno = res;
		return -1;
	}

	return res;
}
