#include <sys/sched.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdbool.h>
#include <memory.h>
#include <malloc.h>
#include "../util-i586/msg.h"
#include "../util-i586/dev.h"

uint16_t* const video_mem = (uint16_t*)0x10000000;
size_t cursor_x = 0, cursor_y = 0;

void putc(char c)
{
	size_t forward = 1;
	switch (c)
	{
		case '\b':
		{
			if (cursor_x)
				cursor_x -= 1;

			c = ' ';
			forward = 0;
			break;
		}
		case '\t':
		{
			size_t new_x = (cursor_x + 4) & ~(4-1);
			cursor_x = (new_x < 80) ? new_x : (++cursor_y, 0);

			break;
		}
		case '\r':
		{
			cursor_x = 0;
			break;
		}
		case '\n':
		{
			cursor_x = 0;
			cursor_y += 1;
			break;
		}
		default:
			break;
	}

	if (c >= ' ')
	{
		video_mem[cursor_y * 80 + cursor_x] = (c | (0x7 << 8));
		cursor_x += forward;
	}

	if (cursor_x >= 80)
	{
		cursor_x = 0;
		cursor_y += 1;
	}

	if (cursor_y >= 25)
	{
		for (size_t i = 0; i != 80*24; ++i)
			video_mem[i] = video_mem[i + 80];

		for (size_t i = 80*24; i != 80*25; ++i)
			video_mem[i] = (' ' | (0x7 << 8));

		cursor_y -= 1;
	}
}

void puts(const char* s)
{
	if (*s == '\e')
	{
		const char* code = s+1;
		if (strcmp(code, "[2J") == 0)
		{
			cursor_x = cursor_y = 0;
			memset(video_mem, 0, 80*25*sizeof(uint16_t));
		}
	}
	else
	{
		while (*s != '\0')
			putc(*s++);
	}
}

int main()
{
	if (pmap(video_mem, 0xB8000, 4096) == NULL)
		return 1;

	memset(video_mem, 0, 80*25*sizeof(uint16_t));
	puts("[tty] Initialized VGA driver.\n");

	puts("[tty] Registering with device manager... ");
	if (dev_register("tty") < 0)
		return 1;

	puts("OK!\n");
	while (true)
	{
		struct msg req;
		if (msg_get_waiting(&req) < 0)
			continue;

		switch (req.code)
		{
			struct msg res;
			msg_create(&res, req.from, -1);

			case 1:
			{
				puts(req.payload.buf);

				res.code = 0;
				send(&res);

				break;
			}
			default:
			{
				send(&res);
				break;
			}
		}

		if (req.payload.size)
			free(req.payload.buf);
	}

	for (;;);
	return 0;
}
