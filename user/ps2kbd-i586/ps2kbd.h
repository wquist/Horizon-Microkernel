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
