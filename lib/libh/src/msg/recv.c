#include <internal/syscall.h>
#include <internal/errno.h>
#include <sys/msg.h>
#include <errno.h>

size_t recv(struct msg* info)
{
	long res = syscall(SYS_recv, info);
	if (__iserr(res))
	{
		errno = res;
		return -1;
	}

	return (size_t)res;
}
