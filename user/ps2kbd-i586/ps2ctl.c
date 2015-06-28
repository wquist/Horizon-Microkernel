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
	byte = sysio(IO_INB,  PS2CTL_PORT_DAT, NULL);

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
	byte = sysio(IO_INB,  PS2CTL_PORT_DAT, NULL);

	cfg.raw = byte;
	cfg.p1_int = 1;

	sysio(IO_OUTB, PS2CTL_PORT_CMD, (void*)PS2CTL_CMD_CFGWR);
	sysio(IO_OUTB, PS2CTL_PORT_DAT, (void*)(unsigned)(cfg.raw));

	return 0;
}

int ps2ctl_has_data()
{
	volatile char byte;
	byte = sysio(IO_INB, PS2CTL_PORT_CMD, NULL);

	union ps2ctl_status stat = { .raw = byte };
	return stat.out_full;
}

char wait_result()
{
	while (!ps2ctl_has_data());

	volatile char byte;
	byte = sysio(IO_INB, PS2CTL_PORT_DAT, NULL);
	return byte;
}