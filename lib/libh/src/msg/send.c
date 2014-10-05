#include <internal/syscall.h>
#include <internal/errno.h>
#include <sys/msg.h>
#include <errno.h>

long send(struct msg* info)
{
	long res = syscall(SYS_send, info);
	if (__iserr(res))
	{
		errno = res;
		return -1;
	}
	
	return res;
}
