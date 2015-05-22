// main.c

#include <horizon/ipc.h>
#include <sys/sched.h>
#include <sys/msg.h>
#include <malloc.h>

int main()
{
	struct msg big = {{0}};
	big.to = 2;

	big.payload.buf  = malloc(5120);
	*(unsigned char*)(big.payload.buf+5000) = 0x95;
	big.payload.size = 5120;

	while (send(&big) == -1);
	return 0;
}
