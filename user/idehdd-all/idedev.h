#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum ata_register ATA_REGISTER;
enum ata_register
{
	ATA_REG_DATA       = 0,
	ATA_REG_ERROR      = 1,
	ATA_REG_SECTOR_CNT = 2,
	ATA_REG_SECTOR     = 3,
	ATA_REG_LCYL       = 4,
	ATA_REG_HCYL       = 5,
	ATA_REG_DRIVE_HEAD = 6,
	ATA_REG_STATUS     = 7,
	ATA_REG_COMMAND    = 7,
	ATA_REG_DEVCTL     = 0x206
};

typedef enum ata_command ATA_COMMAND;
enum ata_command
{
	ATA_CMD_IDENT   = 0xEC,
	ATA_CMD_IDENTPI = 0xA1,
	ATA_CMD_READ    = 0x20,
	ATA_CMD_WRITE   = 0x30
};

typedef struct ata_status ata_status_t;
struct __packed ata_status
{
	uint8_t ERR : 1;
	uint8_t : 2;
	uint8_t DRQ : 1;
	uint8_t : 2;
	uint8_t DRDY : 1;
	uint8_t BSY : 1;
};

typedef struct ata_control ata_control_t;
struct __packed ata_control
{
	uint8_t      : 1;
	uint8_t nIEN : 1;
	uint8_t SRST : 1;
	uint8_t      : 5;
};

typedef struct ide_device ide_device_t;
struct ide_device
{
	bool present;
	bool atapi;
	bool lba;
	bool dma;

	char model[40];
	char serial[20];
	char firmware[8];

	size_t cylinders;
	size_t heads;
	size_t sectors;
	size_t capacity;
};

void idedev_init(ide_device_t* dev, uint16_t cyl);
void idedev_parse_info(ide_device_t* dev, uint16_t* info);
