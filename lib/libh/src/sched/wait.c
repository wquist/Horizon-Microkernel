#include <internal/syscall.h>
#include <internal/errno.h>
#include <sys/sched.h>
#include <errno.h>

long wait(ipcport_t sender)
{
	long res = syscall(SYS_wait, sender);
	if (__iserr(res))
	{
		errno = res;
		return -1;
	}

	return res;
}
