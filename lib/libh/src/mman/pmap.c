#include <internal/syscall.h>
#include <internal/errno.h>
#include <sys/mman.h>
#include <errno.h>

void* pmap(void* dest, uintptr_t src, size_t size)
{
	long res = syscall(SYS_pmap, dest, src, size);
	if (__iserr(res))
	{
		errno = res;
		return NULL;
	}

	return (void*)res;
}
