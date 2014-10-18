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

#include "ctl.h"
#include <arch.h>
#include <stdbool.h>
#include <memory.h>

bool debug_enabled = false;

static size_t cursor_x = 0, cursor_y = 0;
static console_char_t* const buffer = (console_char_t*)CONSOLE_ADDR;

static void scroll();

//! Initialize all formats of debugging.
/*! Any debug_* or d* function can be called after this. */
void debug_init()
{
	debug_enabled = true;

	/* Set the entire screen to spaces instead of 0,
	   since 0 might have extra color info or something
	 */
	size_t len = CONSOLE_WIDTH * CONSOLE_HEIGHT;
	for (size_t i = 0; i != len; ++i)
		buffer[i] = console_format(' ');
}

//! Disable debug output.
/*! debug_trace() calls will no longer be output. */
void debug_disable()
{
	debug_enabled = false;
}

//! Redirect stdio's character stream directory to video memory.
int putchar(char c)
{
	switch (c)
	{
		case '\b':
			if (cursor_x) --cursor_x;
			break;
		case '\t':
			cursor_x = (cursor_x + 4) & -4;
			break;
		case '\r':
			cursor_x = 0;
			break;
		case '\n':
			cursor_x = 0; ++cursor_y;
			break;
		default: break;
	}

	if (c >= ' ')
	{
		console_char_t* loc = &(buffer[cursor_y * CONSOLE_WIDTH + cursor_x]);
		*loc = console_format(c);

		++cursor_x;
	}

	if (cursor_x >= CONSOLE_WIDTH)
	{
		cursor_x = 0;
		++cursor_y;
	}

	if (cursor_y >= CONSOLE_HEIGHT)
		scroll();

	return 0;
}

// Copy each line of the buffer to the previous line, and clear the last line.
void scroll()
{
	size_t i;
	size_t last_row   = CONSOLE_WIDTH * (CONSOLE_HEIGHT-1);
	size_t last_index = CONSOLE_WIDTH *  CONSOLE_HEIGHT;

	for (i = 0; i != last_row;   ++i) buffer[i] = buffer[i + CONSOLE_WIDTH];
	for (;      i != last_index; ++i) buffer[i] = console_format(' ');

	--cursor_y;
}
