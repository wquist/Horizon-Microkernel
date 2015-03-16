#include "fat.h"
#include <sys/msg.h>
#include <sys/sched.h>
#include <string.h>
#include <malloc.h>
#include "../vfsd-all/fs.h"

static uint16_t next_cluster(fat_volume_t* vol, uint16_t current_cluster);
static size_t read_dirent(fat_volume_t* vol, uint16_t cluster, size_t offset, fat_file_t* ret_file);
static int read_sector(int device, size_t sector, uint8_t* buffer);

// REMOVE ME
extern ipcport_t filesystem;

void fat_init(int fd, fat_volume_t* ret_vol)
{
	uint8_t buffer[512];
	read_sector(fd, 0, buffer);

	ret_vol->device = fd;

	/* CHECK FAT TYPE - Only 16 for now */
	ret_vol->format = FAT16;
	memcpy(&(ret_vol->boot_record), buffer, sizeof(fat_boot_record_t));
}

size_t fat_enumerate(fat_volume_t* vol, fat_file_t* parent, size_t index, fat_file_t* ret_file)
{
	fat_boot_record_t* br = &(vol->boot_record);

	size_t cluster;
	if (!parent)
		cluster = 2;
	else
		cluster = parent->cluster;

	size_t cluster_offset = (size_t)(index / 512) * 512;

	size_t cluster_size = 512 * br->cluster_sectors;
	while (index >= cluster_size)
	{
		cluster = next_cluster(vol, cluster);
		if (!cluster)
			return -1;

		index -= cluster_size;
	}

	size_t increment = read_dirent(vol, cluster, index, ret_file);
	return (increment == -1) ? -1 : cluster_offset + increment;
}

uint16_t next_cluster(fat_volume_t* vol, uint16_t current_cluster)
{
	static uint16_t last_sector = 0;
	static uint8_t buffer[512];

	size_t offset = current_cluster * 2;
	size_t sector = vol->boot_record.reserved_sectors + (offset / 512);

	if (sector != last_sector)
	{
		read_sector(vol->device, sector, buffer);
		last_sector = sector;
	}

	size_t entry = offset % 512;
	uint16_t value = *(uint16_t*)&(buffer[entry]);

	return (value >= 0xFFF8) ? 0 : value;
}

size_t read_dirent(fat_volume_t* vol, uint16_t cluster, size_t offset, fat_file_t* ret_file)
{
	static uint16_t last_sector = 0;
	static uint8_t buffer[512];

	size_t first_sector = vol->boot_record.reserved_sectors;
	first_sector += (vol->boot_record.fat_count * vol->boot_record.fat_sectors);

	size_t cluster_base = first_sector + ((cluster - 2) * vol->boot_record.cluster_sectors);

	size_t cluster_size = 512 * vol->boot_record.cluster_sectors;
	while (offset < cluster_size)
	{
		size_t sector = cluster_base + (offset / 512);
		if (sector != last_sector)
		{
			read_sector(vol->device, sector, buffer);
			last_sector = sector;
		}

		size_t sector_offset = offset % 512;
		fat_dirent_t* entry = (fat_dirent_t*)&(buffer[sector_offset]);

		offset += sizeof(fat_dirent_t);

		if (entry->name[0] == FAT_DIREND)
			return -1;
		if (entry->name[0] == FAT_UNUSED)
			continue;
		if (entry->attributes.nibble_low == 0xF || entry->attributes.volume)
			continue;

		char* curr_char = ret_file->name;
		for (size_t i = 0; i != 8; ++i)
		{
			if (entry->name[i] == ' ')
				break;

			*curr_char++ = entry->name[i];
		}

		*curr_char++ = '.';
		for (size_t i = 0; i != 3; ++i)
			*curr_char++ = entry->name[8+i];

		ret_file->type = (entry->attributes.directory) ? VFS_DIR : VFS_FILE;
		ret_file->cluster = entry->cluster_low;
		ret_file->size = entry->size;

		return offset;
	}

	uint16_t next = next_cluster(vol, cluster);
	if (!next)
		return -1;

	return cluster_size + read_dirent(vol, next, 0, ret_file);
}

int read_sector(int device, size_t sector, uint8_t* buffer)
{
	struct msg request = {{0}};
	request.to = filesystem;

	request.code = VFS_READ;
	request.args[0] = device;
	request.args[1] = 512;
	request.args[2] = sector * 512;

	send(&request);
	wait(filesystem);

	struct msg response = {{0}};
	response.payload.buf  = buffer;
	response.payload.size = 512;

	recv(&response);
	return response.code;
}
