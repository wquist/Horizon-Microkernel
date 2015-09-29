#include "ps2kbd.h"
#include <sys/io.h>
#include "ps2ctl.h"
#include "scancodes/generic.h"

enum
{
	STATE_NONE,
	STATE_SKIP,
	STATE_RELEASE
};

const unsigned char* active_map = NULL;
char active_state = STATE_NONE;
char key_states[256] = {0};

int stat_wait(unsigned char cmd, unsigned char dat, unsigned char ret);

int ps2kbd_init()
{
	// Check if present
	if (stat_wait(PS2KBD_CMD_ECHO, -1, PS2KBD_STAT_ECHO) != 0)
		return -1;

	// Reset and test the hardware
	if (stat_wait(PS2KBD_CMD_SELFTEST, -1, PS2KBD_STAT_PASS) != 0)
		return -2;

	return 0;
}

int ps2kbd_mode_set(char mode, const unsigned char* map)
{
	if (mode == 0 || mode > 3)
		return -1;

	// Try to set the mode
	if (stat_wait(PS2KBD_CMD_SCANSET, mode, PS2KBD_STAT_ACK) != 0)
		return -2;

	// Store the scancode map
	active_map = map;

	return 0;
}

unsigned char ps2kbd_read()
{
	unsigned char val;
	val = sysio(IO_INB, PS2KBD_PORT_DAT, NULL);

	if (active_state == STATE_SKIP)
	{
		active_state = STATE_NONE;
		return 0;
	}

	switch (val)
	{
		case PS2KBD_PREFIX_EXT:
		case PS2KBD_PREFIX_EXTSPEC:
		{
			active_state = STATE_SKIP;
			return 0;
		}
		case PS2KBD_PREFIX_RELEASE:
		{
			active_state = STATE_RELEASE;
			return 0;
		}
		default: break;
	}

	unsigned char key = 0;
	if (val < 0x90)
		key = active_map[val];

	if (active_state == STATE_RELEASE)
	{
		if (key < CAPL)
			key_states[key] = 0;

		active_state = STATE_NONE;
		return 0;
	}
	else
	{
		if (key < CAPL)
			key_states[key] = 1;
		else
			key_states[key] = !key_states[key];

		int upper = 0;
		if (key_states[LSHF] || key_states[RSHF])
			upper = !upper;
		if (key_states[CAPL] && IS_LETTER(key))
			upper = !upper;

		if (upper && IS_CHAR(key))
			key = active_map[val + 0x90];

		return key;
	}
}

int stat_wait(unsigned char cmd, unsigned char dat, unsigned char ret)
{
	volatile unsigned char result;

	retry:
	sysio(IO_OUTB, PS2KBD_PORT_CMD, (void*)(unsigned)cmd);
	if (dat != -1)
		sysio(IO_OUTB, PS2KBD_PORT_CMD, (void*)(unsigned)dat);

	while (!ps2ctl_has_data());

	poll:
	result = sysio(IO_INB, PS2KBD_PORT_DAT, NULL);

	if (result == ret)
		return 0;
	else if (result == PS2KBD_STAT_REDO)
		goto retry;
	else if (result == PS2KBD_STAT_ACK)
		goto poll;

	return 0;
}