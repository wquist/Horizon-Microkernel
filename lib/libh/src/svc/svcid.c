#include <internal/syscall.h>
#include <internal/errno.h>
#include <sys/svc.h>
#include <errno.h>

ipcport_t svcid(svc_t sid)
{
	long res = syscall(SYS_svcid, sid);
	if (res == 0)
	{
		errno = ENOTAVAIL;
		return -1;
	}

	return (ipcport_t)res;
}
