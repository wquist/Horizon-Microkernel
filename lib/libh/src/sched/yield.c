#include <internal/syscall.h>
#include <internal/errno.h>
#include <sys/sched.h>
#include <errno.h>

long yield()
{
	long res = syscall(SYS_yield);
	if (__iserr(res))
	{
		errno = res;
		return -1;
	}

	return res;
}
