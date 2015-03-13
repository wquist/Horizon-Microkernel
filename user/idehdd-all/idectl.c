#include <sys/sched.h>
#include <sys/io.h>
#include <memory.h>
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

bool idectl_select(ide_controller_t* ctl, size_t pos)
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

void idectl_identify(ide_controller_t* ctl, size_t pos)
{
	idectl_write(ctl, ATA_REG_SECTOR_CNT, 0x95);
	if (idectl_read(ctl, ATA_REG_SECTOR_CNT) != 0x95)
		return;

	idectl_reset(ctl);
	if (!idectl_select(ctl, pos))
		return;

	uint8_t cl = idectl_read(ctl, ATA_REG_LCYL);
	uint8_t ch = idectl_read(ctl, ATA_REG_HCYL);
	uint8_t st = idectl_read(ctl, ATA_REG_STATUS);

	ide_device_t* dev = &(ctl->devices[pos]);
	idedev_init(dev, cl | (ch << 8), st);

	if (!(dev->present))
		return;

	uint8_t cmd = (dev->atapi) ? ATA_CMD_IDENTPI : ATA_CMD_IDENT;
	idectl_write(ctl, ATA_REG_COMMAND, cmd);

	ata_status_t mask = { .BSY = true,  .DRQ = true };
	ata_status_t val  = { .BSY = false, .DRQ = true };
	idectl_wait(ctl, mask, val);
	/* CHECK FOR ERROR */

	uint16_t info[256];
	for (size_t i = 0; i != 256; ++i)
		info[i] = idectl_read_word(ctl, ATA_REG_DATA);

	idedev_parse_info(dev, info);
}

void idectl_block_io(ide_controller_t* ctl, size_t pos, int mode, size_t start, size_t count, void* buf)
{
	ide_device_t* dev = &(ctl->devices[pos]);
	if (!(dev->present))
		return;

	if (!idectl_select(ctl, pos))
		return;

	uint8_t sector, cl, ch, head;
	if (dev->lba)
	{
		sector = start & 0xFF;
		cl = (start >> 8) & 0xFF;
		ch = (start >> 16) & 0xFF;
		head = (start >> 24) & 0xF;
	}
	else
	{
		size_t cylinder = start / (dev->heads * dev->sectors);
		size_t offset = start % (dev->heads * dev->sectors);

		sector = offset % dev->sectors + 1;
		cl = cylinder & 0xFF;
		ch = (cylinder >> 8) & 0xFF;
		head = offset / dev->sectors;
	}

	uint8_t command = (mode == IDE_READ) ? ATA_CMD_READ : ATA_CMD_WRITE;
	idectl_write(ctl, ATA_REG_SECTOR_CNT, count);
	idectl_write(ctl, ATA_REG_SECTOR, sector);
	idectl_write(ctl, ATA_REG_LCYL, cl);
	idectl_write(ctl, ATA_REG_HCYL, ch);
	idectl_write(ctl, ATA_REG_DRIVE_HEAD, (dev->lba << 6) | (pos << 4) | head);
	idectl_write(ctl, ATA_REG_COMMAND, command);

	ata_status_t mask = { .BSY = true,  .DRQ = true };
	ata_status_t val  = { .BSY = false, .DRQ = true };
	idectl_wait(ctl, mask, val);
	/* CHECK FOR ERROR */

	switch (mode)
	{
		case IDE_WRITE:
		{
			uint16_t* src = (uint16_t*)buf;
			for (size_t i = 0; i != count * 256; ++i)
				idectl_write_word(ctl, ATA_REG_DATA, *src++);

			/* CHECK FOR ERROR */
			break;
		}
		case IDE_READ:
		{
			uint16_t* dest = (uint16_t*)buf;
			for (size_t i = 0; i != count * 256; ++i)
				*dest++ = idectl_read_word(ctl, ATA_REG_DATA);

			break;
		}
	}
}

uint8_t idectl_read(ide_controller_t* ctl, size_t reg)
{
	return sysio(IO_INB, ctl->base + reg, NULL);
}

uint16_t idectl_read_word(ide_controller_t* ctl, size_t reg)
{
	return sysio(IO_INW, ctl->base + reg, NULL);
}

void idectl_write(ide_controller_t* ctl, size_t reg, uint8_t val)
{
	sysio(IO_OUTB, ctl->base + reg, (void*)(unsigned)val);
}

void idectl_write_word(ide_controller_t* ctl, size_t reg, uint16_t val)
{
	sysio(IO_OUTW, ctl->base + reg, (void*)(unsigned)val);
}

bool idectl_wait(ide_controller_t* ctl, ata_status_t mask, ata_status_t val)
{
	const size_t TIMEOUT = 10000;
	size_t elapsed = TIMEOUT;

	uint8_t maskb = mask.raw;
	uint8_t valb  = val.raw;

	while (--elapsed)
	{
		uint8_t status = sysio(IO_INB, ctl->base + ATA_REG_STATUS, NULL);
		if ((status & maskb) == valb)
			break;

		/* WAIT 1 US */
		__asm("pause" ::: "memory");
	}

	return (elapsed) ? true : false;
}
