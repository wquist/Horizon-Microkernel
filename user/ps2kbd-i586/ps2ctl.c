/* Horizon Microkernel - Hobby Operating System
 * Copyright (C) 2014 Wyatt Lindquist
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ps2ctl.h"
#include <sys/io.h>

char wait_result();

int ps2ctl_init()
{
	volatile unsigned char byte;
	union ps2ctl_config cfg;

	// Disable any devices
	sysio(IO_OUTB, PS2CTL_PORT_CMD, (void*)PS2CTL_CMD_DISABLE1);
	sysio(IO_OUTB, PS2CTL_PORT_CMD, (void*)PS2CTL_CMD_DISABLE2);

	// Flush the output buffer
	while (ps2ctl_has_data())
		sysio(IO_INB, PS2CTL_PORT_DAT, &byte);

	// Setup the configuration byte
	sysio(IO_OUTB, PS2CTL_PORT_CMD, (void*)PS2CTL_CMD_CFGRD);
	sysio(IO_INB,  PS2CTL_PORT_DAT, &byte);

	cfg.raw = byte;
	cfg.p1_int    = 0;
	cfg.p2_int    = 0;
	cfg.translate = 0;

	sysio(IO_OUTB, PS2CTL_PORT_CMD, (void*)PS2CTL_CMD_CFGWR);
	sysio(IO_OUTB, PS2CTL_PORT_DAT, (void*)(unsigned)(cfg.raw));

	// Perform the controller self-test
	sysio(IO_OUTB, PS2CTL_PORT_CMD, (void*)PS2CTL_CMD_TESTC);
	if (wait_result() != PS2CTL_RES_TESTC_PASS)
		return -1;

	// Test the first device (ignore second for now)
	sysio(IO_OUTB, PS2CTL_PORT_CMD, (void*)PS2CTL_CMD_TEST1);
	if (wait_result() != PS2CTL_RES_TESTX_PASS)
		return -2;

	// Enable the first device and interrupts
	sysio(IO_OUTB, PS2CTL_PORT_CMD, (void*)PS2CTL_CMD_ENABLE1);

	sysio(IO_OUTB, PS2CTL_PORT_CMD, (void*)PS2CTL_CMD_CFGRD);
	sysio(IO_INB,  PS2CTL_PORT_DAT, &byte);

	cfg.raw = byte;
	cfg.p1_int = 1;

	sysio(IO_OUTB, PS2CTL_PORT_CMD, (void*)PS2CTL_CMD_CFGWR);
	sysio(IO_OUTB, PS2CTL_PORT_DAT, (void*)(unsigned)(cfg.raw));

	return 0;
}

int ps2ctl_has_data()
{
	volatile char byte;
	sysio(IO_INB, PS2CTL_PORT_CMD, &byte);

	union ps2ctl_status stat = { .raw = byte };
	return stat.out_full;
}

char wait_result()
{
	while (!ps2ctl_has_data());

	volatile char byte;
	sysio(IO_INB, PS2CTL_PORT_DAT, &byte);
	return byte;
}
