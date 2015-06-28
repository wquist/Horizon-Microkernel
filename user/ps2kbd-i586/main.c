#include <sys/svc.h>
#include <sys/sched.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdbool.h>
#include <memory.h>
#include <malloc.h>

#include "../util-i586/msg.h"
#include "../util-i586/dev.h"
#include "ps2ctl.h"
#include "ps2kbd.h"
#include "scancodes/mode2_qwerty.h"

#define BUFFER_SIZE 16

char key_buffer[BUFFER_SIZE] = {0};
size_t buffer_pos = 0;
size_t buffer_count = 0;

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
	if (dev_register("kbd") < 0)
		return 1;

	while (true)
	{
		struct msg req;
		if (msg_get_waiting(&req) < 0)
			continue;

		switch (req.from)
		{
			case IPORT_KERNEL:
			{
				char key = ps2kbd_read();
				if (IS_ASCII(key))
				{
					key_buffer[buffer_pos++] = key;
					buffer_pos = buffer_pos % BUFFER_SIZE;

					buffer_count++;
					if (buffer_count >= BUFFER_SIZE)
						buffer_count = BUFFER_SIZE-1;
				}

				break;
			}
			default:
			{
				switch (req.code)
				{
					struct msg res;
					msg_create(&res, req.from, -1);

					case 0:
					{
						size_t count = req.args[0];
						char* buffer = malloc(count);

						size_t i = 0;
						while (buffer_count && count)
						{
							buffer_pos = (buffer_pos) ? buffer_pos - 1 : BUFFER_SIZE-1;
							buffer[i++] = key_buffer[buffer_pos];

							buffer_count -= 1;
							count -= 1;
						}

						res.code = i;
						if (i)
							msg_attach_payload(&res, buffer, i);

						send(&res);
						free(buffer);

						break;
					}
					default:
					{
						send(&res);
						break;
					}
				}
			}
		}
	}

	for (;;);
	return 0;
}
