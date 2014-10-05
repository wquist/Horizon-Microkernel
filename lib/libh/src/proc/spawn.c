#include <internal/syscall.h>
#include <internal/errno.h>
#include <sys/proc.h>
#include <errno.h>

pid_t spawn()
{
	long res = syscall(SYS_spawn);
	if (__iserr(res))
	{
		errno = res;
		return -1;
	}

	return (pid_t)res;
}
