#include <sys/sched.h>
#include <sys/io.h>
#include <stdbool.h>

#include "idectl.h"

void idectl_reset(ide_controller_t* ctl)
{
	ata_control_t ctl_byte = { .SRST = true };
	idectl_write(ctl, ATA_REG_DEVCTL, ctl_byte.raw);

	ata_status_t mask = { .BSY = true };
	ata_status_t val  = { .BSY = true };

	yield(); /* WAIT 2 MS */
	idectl_wait(ctl, mask, val);

	idectl_write(ctl, ATA_REG_DEVCTL, 0);

	val.BSY = false;
	idectl_wait(ctl, mask, val);
}

bool idectl_select(ide_controller* ctl, size_t pos)
{
	ata_status_t status = { .raw = idectl_read(ctl, ATA_REG_STATUS) };
	if (status.BSY || status.DRQ)
		return false;

	idectl_write(ctl, ATA_REG_DRIVE_HEAD, 0xA0 | (pos << 4));

	status.raw = idectl_read(ctl, ATA_REG_STATUS);
	if (status.BSY || status.DRQ)
		return false;

	return true;
}

void idectl_identify(ide_controller* ctl, size_t pos)
{
	idectl_write(ctl, ATA_REG_SECTOR_CNT, 0x95);
	if (idectl_read(ctl, ATA_REG_SECTOR_CNT) != 0x95)
		return;

	idectl_reset(ctl);
	if (!idectl_select(ctl, pos))
		return;

	uint8_t cl = idectl_read(ctl, ATA_REG_LCYL);
	uint8_t ch = idectl_read(ctl, ATA_REG_HCYL);

	ide_device_t* dev = &(ctl->devices[pos]);
	idedev_init(dev, cl | (ch << 8));

	if (!(dev->present))
		return;

	uint8_t cmd = (dev->atapi) ? ATA_CMD_IDENTPI : ATA_CMD_IDENT;
	idectl_write(ctl, ATA_REG_COMMAND, cmd);

	ata_status_t mask = { .BSY = true,  .DRQ = true };
	ata_status_t val  = { .BSY = false, .DRQ = true };
	idectl_wait(ctl, mask, val);
	/* CHECK FOR ERROR */

	uint8_t info[512];
	for (size_t i = 0; i != 512; ++i)
		info[i] = idectl_read(ctl, ATA_REG_DATA);

	idedev_parse_info(dev, (uint16_t*)info);
}

uint8_t idectl_read(ide_controller_t* ctl, size_t reg)
{
	return sysio(IO_INB, ctl->base + reg, NULL);
}

void idectl_write(ide_controller_t* ctl, size_t reg, uint8_t val)
{
	sysio(IO_OUTB, ctl->base + reg, (void*)val);
}

bool idectl_wait(ide_controller_t* ctl, ata_status_t mask, ata_status_t val)
{
	const size_t TIMEOUT = 1000000;
	size_t elapsed = TIMEOUT;

	uint8_t maskb = mask.raw;
	uint8_t valb  = val.raw;

	while (elasped--)
	{
		uint8_t status = sysio(IO_INB, ctl->base + ATA_REG_STATUS, NULL);
		if ((status & maskb) == valb)
			break;

		/* WAIT 1 US */
		__asm("pause" ::: "memory");
	}

	return (elapsed) ? true : false;
}
