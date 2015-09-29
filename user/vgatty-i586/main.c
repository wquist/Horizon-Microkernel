// main.c

#include <horizon/ipc.h>
#include <sys/proc.h>
#include <sys/sched.h>
#include <sys/mman.h>
#include <sys/svc.h>
#include <sys/msg.h>
#include <stdint.h>
#include <stdbool.h>
#include <memory.h>

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

bool register_device()
{
	ipcport_t devmgr;
	while ((devmgr = svcid(SVC_DEVMGR)) == 0);

	struct msg request = {{0}};
	request.to = devmgr;
	request.code = 100;
	request.payload.buf = "tty";
	request.payload.size = 4;

	send(&request);
	wait(devmgr);

	struct msg response = {{0}};
	recv(&response);
	if (response.code != 0)
		return false;

	return true;
}

int main()
{
	if (pmap(video_mem, 0xB8000, 4096) == NULL)
		return 1;

	memset(video_mem, 0, 80*25*sizeof(uint16_t));
	puts("[tty] Initialized VGA driver.\n");

	puts("[tty] Registering with device manager... ");
	if (!register_device())
		return 1;

	puts("OK!\n");

	char buffer[256];
	while (true)
	{
		wait(IPORT_ANY);

		struct msg request = {{0}};
		request.payload.buf  = buffer;
		request.payload.size = 256;

		if (recv(&request) < 0)
		{
			drop(NULL);
			continue;
		}

		struct msg response = {{0}};
		response.to = request.from;
		switch (request.code)
		{
			case 0:
			{
				puts(buffer);
				response.code = 0;

				send(&response);
				break;
			}
			default:
			{
				response.code = -1;

				send(&response);
				break;
			}
		}
	}

	return 0;
}
