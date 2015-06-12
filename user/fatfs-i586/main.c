#include <sys/sched.h>
#include "../util-i586/serial.h"
#include "../util-i586/msg.h"
#include "fat.h"

#define IDEHDD 3

void list_dir(fat_volume_t* vol, fat_file_t* parent)
{
	if (parent && parent->type != 1)
		return;

	size_t iter = 0;
	fat_file_t file;
	while ((iter = fat_enumerate(vol, parent, iter, &file)) != -1)
	{
		serial_write(file.name);
		switch (file.type)
		{
			case 0:
				serial_write(" (file)\n");
				break;
			case 1:
				serial_write(" (directory)\n");
				break;
			default:
				serial_write(" (unknown)\n");
				break;
		}
	}
}

bool find_file(fat_volume_t* vol, fat_file_t* parent, char* name, fat_file_t* ret_file)
{
	if (parent && parent->type != 1)
		return false;

	size_t iter = 0;
	while ((iter = fat_enumerate(vol, parent, iter, ret_file)) != -1)
	{
		if (strcmp(name, ret_file->name) == 0)
			return true;
	}

	return false;
}

void list_file(fat_volume_t* vol, fat_file_t* file)
{
	if (file->type != 0)
		return;

	uint8_t buffer[9] = {0};

	size_t offset = 0;
	while (fat_read(vol, file, offset, 8, buffer) > 0)
	{
		serial_write((const char*)buffer);

		memset(buffer, 0, 8);
		offset += 8;
	}
}

int main()
{
	fat_volume_t vol;
	fat_init(IPORT_GLOBL(IDEHDD), &vol);

	serial_write("\nFAT drive root contents:\n");
	list_dir(&vol, NULL);

	fat_file_t dir;
	find_file(&vol, NULL, "test", &dir);

	serial_write("\n'test' directory contents:\n");
	list_dir(&vol, &dir);

	fat_file_t text;
	find_file(&vol, &dir, "test.txt", &text);

	serial_write("\n'test.txt' contents:\n");
	list_file(&vol, &text);

	for (;;);
	return 0;
}
