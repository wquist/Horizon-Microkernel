// main.c

#include <stdint.h>
#include <memory.h>

int dispatch(void* entry, void* stack) { int ret; __asm("int $0x95" : "=a" (ret) : "a" (2), "b" (entry), "c" (stack)); return ret; }
int vmap(void* dest, size_t size) { int ret; __asm("int $0x95" : "=a" (ret) : "a" (8), "b" (dest), "c" (size)); return ret; }
int pmap(void* dest, void* phys, size_t size) { int ret; __asm("int $0x95" : "=a" (ret) : "a" (8), "b" (dest), "c" (phys), "d" (size)); return ret; }

uint16_t* const video_mem = (void*)0xA0000000;
size_t cursor;

void async();
void print(const char* s);

void main()
{
	pmap(video_mem, (void*)0xB8000, 4096);
	memset(video_mem, 0, 80*25*sizeof(uint16_t));

	print("Hello.");

	vmap((void*)0xB0000000, 4096);
	dispatch(async, (void*)(0xB0000000+4095));

	print("Hello again.");

	for (;;);
}

void async()
{
	print("Hello from thread.");

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
