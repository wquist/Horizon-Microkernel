#include <sys/sched.h>
#include <ctype.h>
#include <malloc.h>
#include "../util-i586/msg.h"
#include "../util-i586/dev.h"
#include "idectl.h"

int main()
{
	ide_controller_t ctl = { .base = 0x1F0 };
	idectl_identify(&ctl, IDE_MASTER);

	if (!(ctl.devices[IDE_MASTER].present))
		return 1;
	if (dev_register("ata") < 0)
		return 1;

	while (true)
	{
		struct msg req;
		if (msg_get_waiting(&req) < 0)
			continue;

		switch (req.code)
		{
			struct msg res;
			msg_create(&res, req.from, -1);

			case 0:
			{
				size_t len = req.args[0] / 512;
				size_t off = req.args[1] / 512;

				void* buf = malloc(len * 512);
				idectl_block_io(&ctl, IDE_MASTER, IDE_READ, off, len, buf);

				res.code = len * 512;
				msg_attach_payload(&res, buf, len * 512);

				send(&res);

				free(buf);
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
