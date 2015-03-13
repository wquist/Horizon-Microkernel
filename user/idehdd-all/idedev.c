#include <memory.h>
#include "idedev.h"

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

	memcpy(dev->model, &(info[27]), 39);
	memcpy(dev->serial, &(info[10]), 19);
	memcpy(dev->firmware, &(info[23]), 7);
}
