// main.c

#include <horizon/ipc.h>
#include <sys/proc.h>
#include <sys/sched.h>
#include <sys/svc.h>
#include <sys/msg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "ps2ctl.h"
#include "ps2kbd.h"
#include "scancodes/mode2_qwerty.h"

#define BUFFER_SIZE 16

char key_buffer[BUFFER_SIZE] = {0};
size_t buffer_pos = 0;
size_t buffer_count = 0;

bool register_device()
{
	ipcport_t devmgr;
	while ((devmgr = svcid(SVC_DEVMGR)) == 0);

	struct msg request = {{0}};
	request.to = devmgr;
	request.code = 1;
	request.payload.buf = "kbd";
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
	if (ps2ctl_init() < 0)
		return 1;
	if (ps2kbd_init() < 0)
		return 1;

	if (ps2kbd_mode_set(2, scanmode2_qwerty) < 0)
		return 1;

	if (svcown(SVC_IRQ(1)) < 0)
		return 1;
	if (!register_device())
		return 1;

	while (true)
	{
		wait(IPORT_ANY);

		struct msg request = {{0}};
		if (recv(&request) < 0)
		{
			drop(NULL);
			continue;
		}

		switch (request.from)
		{
			case IPORT_KERNEL:
			{
				char key = ps2kbd_read();
				if (IS_ASCII(key))
				{
					key_buffer[buffer_pos++] = key;
					buffer_pos = buffer_pos % BUFFER_SIZE;

					buffer_count += 1;
					if (buffer_count >= BUFFER_SIZE)
						buffer_count = BUFFER_SIZE-1;
				}

				break;
			}
			default:
			{
				switch (request.code)
				{
					case 1:
					{
						size_t count = request.args[0];
						if (!count || count >= BUFFER_SIZE)
						{
							struct msg response = {{0}};
							response.to = request.from;
							response.code = -1;

							send(&response);
							break;
						}

						char buffer[BUFFER_SIZE] = {0};
						size_t i = 0;
						while (buffer_count && count)
						{
							if (buffer_pos == 0)
								buffer_pos = BUFFER_SIZE-1;
							else
								buffer_pos -= 1;

							buffer[i++] = key_buffer[buffer_pos];
							buffer_count -= 1;
						}

						struct msg response = {{0}};
						response.to = request.from;
						response.code = 0;

						if (i)
						{
							response.payload.buf  = buffer;
							response.payload.size = i;
						}

						send(&response);
						break;
					}
					default:
					{
						struct msg response = {{0}};
						response.to = request.from;
						response.code = -1;

						send(&response);
						break;
					}
				}

				break;
			}
		}
	}

	return 0;
}
