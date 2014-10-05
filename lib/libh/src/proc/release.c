#include <internal/syscall.h>
#include <internal/errno.h>
#include <sys/proc.h>
#include <errno.h>

long release(tid_t tid)
{
	long res = syscall(SYS_release, tid);
	if (__iserr(res))
	{
		errno = res;
		return -1;
	}

	return res;
}
