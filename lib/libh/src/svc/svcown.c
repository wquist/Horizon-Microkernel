#include <internal/syscall.h>
#include <internal/errno.h>
#include <sys/svc.h>
#include <errno.h>

long svcown(svc_t sid)
{
	long res = syscall(SYS_svcown, sid);
	if (__iserr(res))
	{
		errno = res;
		return -1;
	}

	return res;
}
