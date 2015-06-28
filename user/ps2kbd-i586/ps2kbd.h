#pragma once

enum
{
	PS2KBD_PORT_CMD = 0x60,
	PS2KBD_PORT_DAT = 0x60
};

enum
{
	PS2KBD_CMD_ECHO     = 0xEE,
	PS2KBD_CMD_SCANSET  = 0xF0,
	PS2KBD_CMD_SELFTEST = 0xFF
};

enum
{
	PS2KBD_STAT_PASS = 0xAA,
	PS2KBD_STAT_ECHO = 0xEE,
	PS2KBD_STAT_ACK  = 0xFA,
	PS2KBD_STAT_REDO = 0xFE
};

enum
{
	PS2KBD_PREFIX_EXT     = 0xE0,
	PS2KBD_PREFIX_EXTSPEC = 0xE1,
	PS2KBD_PREFIX_RELEASE = 0xF0
};

int ps2kbd_init();
int ps2kbd_mode_set(char mode, const unsigned char* map);

unsigned char ps2kbd_read();