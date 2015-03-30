#include "fat.h"
#include <sys/msg.h>
#include <sys/sched.h>
#include <ctype.h> //< FIXME: _tolower and _toupper are swapped.
#include <string.h>
#include <malloc.h>
#include "../vfsd-all/fs.h"

static uint16_t next_cluster(fat_volume_t* vol, uint16_t current_cluster);
static uint16_t find_cluster(fat_volume_t* vol, uint16_t cluster, size_t* offset);
static size_t get_cluster_sector(fat_volume_t* vol, uint16_t cluster);
static size_t read_dirent(fat_volume_t* vol, uint16_t cluster, size_t offset, fat_file_t* ret_file);
static int read_sector(fat_volume_t* vol, size_t sector, uint8_t* buffer);

void fat_init(ipcport_t device, int fd, fat_volume_t* ret_vol)
{
	ret_vol->device = device;
	ret_vol->fd = fd;

	uint8_t buffer[512];
	read_sector(ret_vol, 0, buffer);

	/* CHECK FAT TYPE - Only 16 for now */
	ret_vol->format = FAT16;
	memcpy(&(ret_vol->boot_record), buffer, sizeof(fat_boot_record_t));
}

size_t fat_enumerate(fat_volume_t* vol, fat_file_t* parent, size_t index, fat_file_t* ret_file)
{
	size_t cluster;
	if (!parent)
		cluster = 0;
	else
		cluster = parent->cluster;

	size_t offset = index;
	cluster = find_cluster(vol, cluster, &offset);
	size_t absolute = index - offset;

	size_t increment = read_dirent(vol, cluster, offset, ret_file);
	return (increment == -1) ? -1 : absolute + increment;
}

size_t fat_read(fat_volume_t* vol, fat_file_t* file, size_t off, size_t len, uint8_t* buffer)
{
	if (off >= file->size)
		return 0;
	if (off + len > file->size)
		len = file->size - off;

	size_t cluster = file->cluster;
	size_t total_end = off + len;
	while (off < total_end)
	{
		cluster = find_cluster(vol, cluster, &off);
		size_t cluster_sector = get_cluster_sector(vol, cluster);
		size_t target_sector = cluster_sector + (off / 512);

		uint8_t full_buffer[512];
		read_sector(vol, target_sector, full_buffer);

		size_t to_read = total_end - off;
		if (to_read > 512)
			to_read = 512;

		memcpy(buffer, full_buffer, to_read);

		off += to_read;
		buffer += to_read;
	}

	return len;
}

uint16_t next_cluster(fat_volume_t* vol, uint16_t current_cluster)
{
	static uint16_t last_sector = -1;
	static uint8_t buffer[512];

	size_t offset = current_cluster * 2;
	size_t sector = vol->boot_record.reserved_sectors + (offset / 512);

	if (sector != last_sector)
	{
		read_sector(vol, sector, buffer);
		last_sector = sector;
	}

	size_t entry = offset % 512;
	uint16_t value = *(uint16_t*)&(buffer[entry]);

	return (value >= 0xFFF8) ? 0 : value;
}

uint16_t find_cluster(fat_volume_t* vol, uint16_t cluster, size_t* offset)
{
	size_t cluster_size = 512 * vol->boot_record.cluster_sectors;
	while (*offset >= cluster_size)
	{
		cluster = next_cluster(vol, cluster);
		if (!cluster)
			return 0;

		*offset -= cluster_size;
	}

	return cluster;
}

size_t get_cluster_sector(fat_volume_t* vol, uint16_t cluster)
{
	fat_boot_record_t* br = &(vol->boot_record);

	size_t first_sector = br->reserved_sectors;
	first_sector += (br->fat_count * br->fat_sectors);
	if (cluster == 0)
		return first_sector;

	size_t root_sectors = ((br->dirent_count * 32) + br->sector_bytes - 1) / br->sector_bytes;
	size_t cluster_start = (cluster - 2) * br->cluster_sectors;
	return first_sector + root_sectors + cluster_start;
}

size_t read_dirent(fat_volume_t* vol, uint16_t cluster, size_t offset, fat_file_t* ret_file)
{
	static uint16_t last_sector = -1;
	static uint8_t buffer[512];

	char long_filename[256] = {0};
	while (true)
	{
		size_t cluster_sector = get_cluster_sector(vol, cluster);
		size_t target_sector = cluster_sector + (offset / 512);

		if (target_sector != last_sector)
		{
			read_sector(vol, target_sector, buffer);
			last_sector = target_sector;
		}

		size_t sector_offset = offset % 512;
		fat_dirent_t* entry = (fat_dirent_t*)&(buffer[sector_offset]);
		offset += sizeof(fat_dirent_t);

		if (entry->name[0] == FAT_DIREND)
			return -1;
		if (entry->name[0] == FAT_UNUSED)
			continue;

		if (entry->attributes.nibble_low == 0xF)
		{
			fat_dirent_long_t* long_entry = (fat_dirent_long_t*)entry;

			size_t pos = long_entry->order & 0x3F;
			char* name_buf = &(long_filename[(pos - 1) * 13]);

			int i;
			for (i = 0; i != 5; ++i)
				*name_buf++ = (char)(long_entry->name_low[i]);
			for (i = 0; i != 6; ++i)
				*name_buf++ = (char)(long_entry->name_middle[i]);
			for (i = 0; i != 2; ++i)
				*name_buf++ = (char)(long_entry->name_high[i]);

			continue;
		}

		if (entry->attributes.volume)
			continue;

		if (*long_filename)
		{
			strncpy(ret_file->name, long_filename, 32);
			ret_file->name[31] = '\0';
		}
		else
		{
			char* curr_char = ret_file->name;
			for (size_t i = 0; i != 8; ++i)
			{
				if (entry->name[i] == ' ')
					break;

				*curr_char++ = _toupper(entry->name[i]);
			}

			if (!(entry->attributes.directory))
			{
				*curr_char++ = '.';
				for (size_t i = 0; i != 3; ++i)
					*curr_char++ = _toupper(entry->name[8+i]);
			}

			*curr_char = '\0';
		}

		ret_file->type = (entry->attributes.directory) ? VFS_DIR : VFS_FILE;
		ret_file->cluster = entry->cluster_low;
		ret_file->size = entry->size;

		return offset;
	}
}

int read_sector(fat_volume_t* vol, size_t sector, uint8_t* buffer)
{
	struct msg request = {{0}};
	request.to = vol->device;

	request.code = VFS_FSREAD;
	request.args[0] = vol->fd;
	request.args[1] = 512;
	request.args[2] = sector * 512;

	send(&request);
	wait(request.to);

	struct msg response = {{0}};
	response.payload.buf  = buffer;
	response.payload.size = 512;

	recv(&response);
	return response.code;
}
