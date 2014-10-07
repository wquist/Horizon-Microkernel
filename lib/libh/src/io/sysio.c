#include <internal/syscall.h>
#include <internal/errno.h>
#include <sys/io.h>
#include <errno.h>

long sysio(sysop_t action, unsigned long arg, volatile void* param)
{
	long res = syscall(SYS_sysio, action, arg, param);
	if (__iserr(res))
	{
		errno = res;
		return -1;
	}

	return res;
}
