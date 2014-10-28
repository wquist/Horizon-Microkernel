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

#define IS_ASCII(x)  (((unsigned char)(x) > 0)  && ((unsigned char)(x) < 128))
#define IS_CHAR(x)   (((unsigned char)(x) > 32) && ((unsigned char)(x) < 128))
#define IS_LETTER(x) (((unsigned char)(x) > 96) && ((unsigned char)(x) < 123))

enum
{
	ESC = 128,

	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,

	LSHF,
	LCTL,
	LALT,
	RSHF,
	RCTL,
	RALT,

	CAPL,
	NUML,
	SCRL
};
