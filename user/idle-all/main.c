#include <sys/svc.h>

int main()
{
	svcown(SVC_PAGER);

	for (;;);
	return 0;
}
