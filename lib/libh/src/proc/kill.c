#include <internal/syscall.h>
#include <internal/errno.h>
#include <sys/proc.h>
#include <errno.h>

long kill(pid_t pid)
{
	long res = syscall(SYS_kill, pid);
	if (__iserr(res))
	{
		errno = res;
		return -1;
	}

	return res;
}
