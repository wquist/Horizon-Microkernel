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

/*! \file arch/i586/hw/console.h
 *  \date June 2014
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#define CONSOLE_ADDR   ((void*)0xB8000)
#define CONSOLE_WIDTH  (80U)
#define CONSOLE_HEIGHT (25U)

typedef uint16_t console_char_t;

enum console_color
{
	CONSOLE_COLOR_BLACK = 0x0,
	CONSOLE_COLOR_BLUE,
	CONSOLE_COLOR_GREEN,
	CONSOLE_COLOR_CYAN,
	CONSOLE_COLOR_RED,
	CONSOLE_COLOR_MAGENTA,
	CONSOLE_COLOR_BROWN,
	CONSOLE_COLOR_GRAY,

	CONSOLE_COLOR_BRIGHTER = (1 << 4)
};

static inline console_char_t console_format(char c)
{
	return (console_char_t)(c | (CONSOLE_COLOR_GRAY << 8));	
}
