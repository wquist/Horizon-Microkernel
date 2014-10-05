#include <internal/syscall.h>
#include <internal/errno.h>
#include <sys/shm.h>
#include <errno.h>

shmid_t share(struct shm* info)
{
	long res = syscall(SYS_share, info);
	if (__iserr(res))
	{
		errno = res;
		return -1;
	}

	return (shmid_t)res;
}
