#include <sys/sched.h>
#include <ctype.h>
#include <malloc.h>
#include "../util-i586/serial.h"
#include "../util-i586/msg.h"
#include "idectl.h"

int main()
{
	ide_controller_t ctl = { .base = 0x1F0 };
	idectl_identify(&ctl, IDE_MASTER);

	if (!(ctl.devices[IDE_MASTER].present))
	{
		serial_write("No IDE device found.\n");
		return 1;
	}

	ide_device_t* dev = &(ctl.devices[IDE_MASTER]);
	serial_write("\n");

	serial_write(dev->model);    serial_write("\n");
	serial_write(dev->serial);   serial_write("\n");
	serial_write(dev->firmware); serial_write("\n");

	char block0[512] = {0};
	idectl_block_io(&ctl, IDE_MASTER, IDE_READ, 0, 1, block0);

	for (size_t i = 0; i != 512; ++i)
	{
		char str[] = { block0[i], '\0' };
		if (!isalpha(*str) && !isdigit(*str))
			serial_write(".");
		else
			serial_write(str);

		if (i%16 == 15)
			serial_write("\n");
	}

	size_t sz;
	while (true)
	{
		wait(IPORT_ANY);

		sz = peek();
		if (sz < 0)
		{
			//
			continue;
		}
		if (sz > 0)
		{
			drop(NULL);
			continue;
		}

		struct msg req;
		recv(&req);

		struct msg res;
		switch (req.code)
		{
			case 0:
			{
				size_t len = req.args[0] / 512;
				size_t off = req.args[1] / 512;

				void* buf = malloc(len * 512);
				idectl_block_io(&ctl, IDE_MASTER, IDE_READ, off, len, buf);

				msg_create(&res, req.from, len * 512);
				msg_attach_payload(&res, buf, len * 512);

				send(&res);

				free(buf);
				break;
			}
			default:
			{
				msg_create(&res, req.from, -1);

				send(&res);
				break;
			}
		}
	}

	for (;;);
	return 0;
}
