#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "idedev.h"

enum
{
	IDE_MASTER = 0,
	IDE_SLAVE
};

enum
{
	IDE_READ = 0,
	IDE_WRITE
};

typedef struct ide_controller ide_controller_t;
struct ide_controller
{
	uint16_t base;
	ide_device_t devices[2];
};

void idectl_reset(ide_controller_t* ctl);
bool idectl_select(ide_controller_t* ctl, bool pos);
void idectl_identify(ide_controller_t* ctl, bool pos);

void idectl_block_io(ide_controller_t* ctl, bool pos, int mode, size_t start, size_t count, void* buf);

uint8_t idectl_read(ide_controller_t* ctl, size_t reg);
uint16_t idectl_read_word(ide_controller_t* ctl, size_t reg);
void idectl_write(ide_controller_t* ctl, size_t reg, uint8_t val);
void idectl_write_word(ide_controller_t* ctl, size_t reg, uint16_t val);

bool idectl_wait(ide_controller_t* ctl, ata_status_t mask, ata_status_t val);