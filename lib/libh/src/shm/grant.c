#include <internal/syscall.h>
#include <internal/errno.h>
#include <sys/shm.h>
#include <errno.h>

long grant(struct shm* info, uintptr_t dest)
{
	long res = syscall(SYS_grant, info, dest);
	if (__iserr(res))
	{
		errno = res;
		return -1;
	}

	return res;
}
