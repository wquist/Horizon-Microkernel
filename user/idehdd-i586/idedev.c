#include <memory.h>
#include "idedev.h"

static void read_string(char* dest, char* src, size_t len);

void idedev_init(ide_device_t* dev, uint16_t cyl, uint8_t status)
{
	memset(dev, 0, sizeof(ide_device_t));

	if (cyl == 0xEB14)
	{
		dev->present = true;
		dev->atapi = true;
	}
	else if (cyl == 0x0 && status != 0x0)
	{
		dev->present = true;
	}
}

void idedev_parse_info(ide_device_t* dev, uint16_t* info)
{
	dev->cylinders = info[1];
	dev->heads = info[3];
	dev->sectors = info[6];

	dev->dma = (info[49] >> 8) & 0x1;
	dev->lba = (info[49] >> 9) & 0x1;

	if (dev->lba)
		dev->capacity = info[60];
	else
		dev->capacity = dev->heads * dev->sectors * dev->cylinders;

	read_string(dev->model, (char*)&(info[27]), 39);
	read_string(dev->serial, (char*)&(info[10]), 19);
	read_string(dev->firmware, (char*)&(info[23]), 7);
}

void idedev_get_geometry(ide_device_t* dev, bool pos, size_t start, ata_geometry_t* geom)
{
	if (dev->lba)
	{
		geom->sector = start & 0xFF;
		geom->cylinder.low  = (start >> 8)  & 0xFF;
		geom->cylinder.high = (start >> 16) & 0xFF;

		geom->head = (start >> 24) & 0xF;
	}
	else
	{
		size_t cylinder = start / (dev->heads * dev->sectors);
		size_t offset = start % (dev->heads * dev->sectors);

		geom->sector = offset % dev->sectors + 1;
		geom->cylinder.low  = (cylinder) & 0xFF;
		geom->cylinder.high = (cylinder >> 8) & 0xFF;

		geom->head = offset / dev->sectors;
	}

	geom->head |= (pos << 4);
	geom->head |= (dev->lba << 6);
}

void read_string(char* dest, char* src, size_t len)
{
	for (size_t i = 0; i != len; ++i, ++src, ++dest)
	{
		*dest = (i % 2) ? src[-1] : src[1];
		if (*dest < 32 || *dest > 126)
		{
			*dest = '\0';
			break;
		}
	}
}
