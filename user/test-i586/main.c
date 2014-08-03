// main.c

#include <stdint.h>
#include <memory.h>

int vmap(void* dest, void* phys, size_t size) { int ret; __asm("int $0x95" : "=a" (ret) : "a" (8), "b" (dest), "c" (phys), "d" (size)); return ret; }

uint16_t* const video_mem = (void*)0xA0000000;
size_t cursor;

void print(const char* s);

void main()
{
	int res = vmap(video_mem, (void*)0xB8000, 4096);
	if (res != 0)
		goto done;

	memset(video_mem, 0, 80*25*sizeof(uint16_t));
	print("Hello.");

done:
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
