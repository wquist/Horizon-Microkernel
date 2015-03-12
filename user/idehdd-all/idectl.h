#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "idedev.h"

typedef enum ide_devpos IDE_DEVPOS;
enum ide_devpos
{
	IDE_MASTER = 0,
	IDE_SLAVE
}

typedef struct ide_controller ide_controller_t;
struct ide_controller
{
	uint16_t base;
	ide_device_t devices[2];
};

void idectl_reset(ide_controller_t* ctl);
bool idectl_select(ide_controller* ctl, size_t pos);
void idectl_identify(ide_controller* ctl, size_t pos);

uint8_t idectl_read(ide_controller_t* ctl, size_t reg);
void idectl_write(ide_controller_t* ctl, size_t reg, uint8_t val);
bool idectl_wait(ide_controller_t* ctl, ata_status_t mask, ata_status_t val);
