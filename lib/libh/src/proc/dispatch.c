#include <internal/syscall.h>
#include <internal/errno.h>
#include <sys/proc.h>
#include <errno.h>

tid_t dispatch(void* entry, void* stack)
{
	long res = syscall(SYS_dispatch, entry, stack);
	if (__iserr(res))
	{
		errno = res;
		return -1;
	}

	return (tid_t)res;
}
