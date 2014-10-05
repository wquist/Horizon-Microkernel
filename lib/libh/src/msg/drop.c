#include <internal/syscall.h>
#include <internal/errno.h>
#include <sys/msg.h>
#include <errno.h>

long drop(struct msg* info)
{
	long res = syscall(SYS_drop, info);
	if (__iserr(res))
	{
		errno = res;
		return -1;
	}

	return res;
}
