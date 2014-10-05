#include <internal/syscall.h>
#include <internal/errno.h>
#include <sys/proc.h>
#include <errno.h>

long launch(pid_t pid, uintptr_t entry)
{
	long res = syscall(SYS_launch, pid, entry);
	if (__iserr(res))
	{
		errno = res;
		return -1;
	}

	return res;
}
