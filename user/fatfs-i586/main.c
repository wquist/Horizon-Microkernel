#include <sys/sched.h>
#include "../util-i586/serial.h"
#include "../util-i586/msg.h"
#include "fat.h"

#define IDEHDD 3

int main()
{
	fat_volume_t vol;
	fat_init(IPORT_GLOBL(IDEHDD), &vol);

	serial_write("\nFAT drive root contents:\n");

	size_t iter;
	fat_file_t file;
	while ((iter = fat_enumerate(&vol, NULL, iter, &file)) != -1)
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

	for (;;);
	return 0;
}
