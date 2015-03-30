#pragma once

#include <horizon/ipc.h>
#include <stdint.h>
#include <stdbool.h>

enum
{
	FAT12,
	FAT16,
	FAT32
};

enum
{
	FAT_UNUSED = 0xE5,
	FAT_DIREND = 0x0
};

typedef struct fat_extended_record fat_extended_record_t;
struct __packed fat_extended_record
{
	uint8_t drive;
	uint8_t flags;
	uint8_t signature;

	uint32_t volume_id;
	uint8_t  volume_label[11];

	uint8_t filesystem[8];
};

typedef struct fat_boot_record fat_boot_record_t;
struct __packed fat_boot_record
{
	uint8_t jmp[3];
	uint8_t oem[8];

	uint16_t sector_bytes;
	uint8_t  cluster_sectors;
	uint16_t reserved_sectors;

	uint8_t  fat_count;
	uint16_t dirent_count;
	uint16_t sector_count;

	uint8_t media_type;

	uint16_t fat_sectors;
	uint16_t track_sectors;
	uint16_t heads;
	uint32_t hidden_sectors;
	uint32_t large_sectors;

	union
	{
		fat_extended_record_t extended;
	};
};

typedef struct fat_dirent fat_dirent_t;
struct __packed fat_dirent
{
	uint8_t name[11];
	union
	{
		struct __packed
		{
			uint8_t read_only : 1;
			uint8_t hidden    : 1;
			uint8_t system    : 1;
			uint8_t volume    : 1;
			uint8_t directory : 1;
			uint8_t archive   : 1;
			uint8_t           : 2;
		};
		struct __packed
		{
			uint8_t nibble_low  : 4;
			uint8_t nibble_high : 4;
		};
		uint8_t raw;
	} attributes;
	uint8_t reserved;

	uint8_t  create_precise;
	uint16_t create_time;
	uint16_t create_date;
	uint16_t access_date;

	uint16_t cluster_high;

	uint16_t modify_time;
	uint16_t modify_date;

	uint16_t cluster_low;
	uint32_t size;
};

typedef struct fat_dirent_long fat_dirent_long_t;
struct __packed fat_dirent_long
{
	uint8_t order;
	uint16_t name_low[5];

	uint8_t attributes;
	uint8_t type;
	uint8_t checksum;

	uint16_t name_middle[6];
	uint16_t reserved;
	uint16_t name_high[2];
};

typedef struct fat_volume fat_volume_t;
struct fat_volume
{
	ipcport_t device;
	int fd;

	size_t format;
	fat_boot_record_t boot_record;
};

typedef struct fat_file fat_file_t;
struct fat_file
{
	char name[32];
	int type;

	size_t cluster;
	size_t size;
};

void fat_init(ipcport_t device, int fd, fat_volume_t* ret_vol);

size_t fat_enumerate(fat_volume_t* vol, fat_file_t* parent, size_t index, fat_file_t* ret_file);
size_t fat_read(fat_volume_t* vol, fat_file_t* file, size_t off, size_t len, uint8_t* buffer);
