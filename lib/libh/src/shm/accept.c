#include <internal/syscall.h>
#include <internal/errno.h>
#include <sys/shm.h>
#include <errno.h>

size_t accept(shmid_t sid, void* dest, size_t size)
{
	long res = syscall(SYS_accept, sid, dest, size);
	if (__iserr(res))
	{
		errno = res;
		return -1;
	}

	return (size_t)res;
}
