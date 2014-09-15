// main.c

#include <horizon/ipc.h>
#include <horizon/msg.h>
#include <stdint.h>
#include <memory.h>

int dispatch(void* entry, void* stack) { int ret; __asm("int $0x95" : "=a" (ret) : "a" (2), "b" (entry), "c" (stack)); return ret; }

int wait(ipcport_t wait_for) { int ret; __asm("int $0x95" : "=a" (ret) : "a" (6), "b" (wait_for)); return ret; }

int vmap(void* dest, size_t size) { int ret; __asm("int $0x95" : "=a" (ret) : "a" (7), "b" (dest), "c" (size)); return ret; }
int pmap(void* dest, void* phys, size_t size) { int ret; __asm("int $0x95" : "=a" (ret) : "a" (8), "b" (dest), "c" (phys), "d" (size)); return ret; }

int send(struct msg* msg) { int ret; __asm("int $0x95" : "=a" (ret) : "a" (13), "b" (msg)); return ret; }
int recv(struct msg* msg) { int ret; __asm("int $0x95" : "=a" (ret) : "a" (14), "b" (msg)); return ret; }

uint16_t* const video_mem = (void*)0xA0000000;
size_t cursor = 0;

void async();
void print(const char* s);

void main()
{
	pmap(video_mem, (void*)0xB8000, 4096);
	memset(video_mem, 0, 80*25*sizeof(uint16_t));

	vmap((void*)0xB0000000, 4096);
	dispatch(async, (void*)(0xB0000000+4095));

	print("Hello.");

	wait(IPORT_ANY);
	print("Awoken.");

	uint32_t extra = 0;

	struct msg m = {{0}};
	m.payload.buf  = &extra;
	m.payload.size = 4;

	recv(&m);
	print("Received.");

	if (extra == 5)
		print("Read payload.");

	print("Sending reply.");
	struct msg rep = {{0}};
	rep.to = m.from;

	int res = send(&rep);
	if (res < 0)
		print("An error occurred.");
	else
		print("Reply sent.");

	for (;;);
}

void async()
{
	print("Hello from thread.");

	uint32_t extra = 5;

	struct msg m = {{0}};
	m.to   = IPORT_LOCAL(2);
	m.code = 1;
	m.payload.buf  = &extra;
	m.payload.size = 4;

	print("Sending.");
	send(&m);
	print("Sent.");

	print("Waiting for reply.");
	wait(IPORT_ANY);

	recv(&m);
	print("Received.");

	for (;;);
}

void print(const char* s)
{
	while (*s != '\0')
	{
		video_mem[cursor] = (*s | (0x7 << 8));
		++cursor, ++s;
	}

	cursor += 80;
	cursor -= (cursor % 80);
}
