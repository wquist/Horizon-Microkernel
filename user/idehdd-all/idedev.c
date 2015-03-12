#include "idedev.h"

void idedev_init(ide_device_t* dev, uint16_t cyl)
{
	memset(dev, 0, sizeof(ide_device_t));

	if (cyl == 0xEB14)
	{
		dev->present = true;
		dev->atapi = true;
	}
	else if (cyl == 0x0)
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
}
