#pragma once

#include <sys/io.h>
#include <stdint.h>

// Write to COM1 serial port.
static void serial_write(const char* str)
{
	while (*str)
	{
		sysio(IO_OUTB, 0x3F8, (void*)(unsigned)*str);
		str += 1;
	}
}
