#include <internal/syscall.h>
#include <internal/errno.h>
#include <sys/mman.h>
#include <errno.h>

void* vmap(void* dest, size_t size)
{
	long res = syscall(SYS_vmap, dest, size);
	if (__iserr(res))
	{
		errno = res;
		return NULL;
	}

	return (void*)res;
}
