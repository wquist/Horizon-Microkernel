#include <sys/sched.h>
#include <sys/proc.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include "../util-i586/serial.h"
#include "../util-i586/msg.h"
#include "fat.h"

#define IDEHDD 3
#define FREEPG (void*)0xA0000000

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

	fat_file_t bin;
	find_file(&vol, NULL, "main.bin", &bin);

	serial_write("\nReading binary file...\n");

	uint8_t bin_contents[256];
	fat_read(&vol, &bin, 0, 256, bin_contents);

	serial_write("Spawning new process...\n");
	pid_t pid = spawn();

	serial_write("Mapping data to memory...\n");
	vmap(FREEPG, 4096);
	memcpy(FREEPG, bin_contents, 256);

	serial_write("Transferring memory to new process...\n");
	struct shm code =
	{
		.to   = IPORT_GLOBL(pid),
		.addr = FREEPG,
		.size = 4096,
		.prot = SPROT_READ | SPROT_WRITE
	};
	grant(&code, 0x1000000);

	serial_write("Launching new process...\n");
	launch(pid, 0x1000000);

	for (;;);
	return 0;
}
