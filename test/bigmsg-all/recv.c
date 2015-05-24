// recv.c

#include <horizon/ipc.h>
#include <sys/sched.h>
#include <sys/msg.h>
#include <malloc.h>

int main()
{
	struct msg big = {{0}};

	big.payload.buf  = malloc(5120);
	big.payload.size = 5120;

	while (recv(&big) != 5120);
	if (*(unsigned char*)(big.payload.buf+5000) == 0x95)
		*(char*)0x1 = 0;

	return 0;
}
