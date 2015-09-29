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

#include "generic.h"

static const unsigned char scanmode2_qwerty[] = 
{
/*   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F  */
// Lower case:
	 0,  F9,   0,  F5,  F3,  F1,  F2,  F12,  0,  F10, F8,  F6,  F4, '\t', '`',  0,
	 0,  LALT,LSHF, 0,  LCTL,'q', '1',  0,   0,   0,  'z', 's', 'a', 'w', '2',  0,
	 0,  'c', 'x', 'd', 'e', '4', '3',  0,   0,  ' ', 'v', 'f', 't', 'r', '5',  0,
	 0,  'n', 'b', 'h', 'g', 'y', '6',  0,   0,   0,  'm', 'j', 'u', '7', '8',  0,
	 0,  ',', 'k', 'i', 'o', '0', '9',  0,   0,  '.', '/', 'l', ';', 'p', '-',  0,
	 0,   0, '\'',  0,  '[', '=',  0,   0,  CAPL,RSHF,'\n', ']',  0, '\\', 0,   0,
	 0,   0,   0,   0,   0,   0, '\b',  0,   0,  '1',  0,  '4', '7',  0,   0,   0,
	'0', '.', '2', '5', '6', '8', ESC, NUML,F11, '+', '3', '-', '*', '9', SCRL, 0,
	 0,   0,   0,   0,   0,   0,   0,  F7,   0,   0,   0,   0,   0,   0,   0,   0,

// Upper case:
	 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  '~',  0,
	 0,   0,   0,   0,   0,  'Q', '!',  0,   0,   0,  'Z', 'S', 'A', 'W', '@',  0,
	 0,  'C', 'X', 'D', 'E', '$', '#',  0,   0,  ' ', 'V', 'F', 'T', 'R', '%',  0,
	 0,  'N', 'B', 'H', 'G', 'Y', '^',  0,   0,   0,  'M', 'J', 'U', '&', '*',  0,
	 0,  '<', 'K', 'I', 'O', ')', '(',  0,   0,  '>', '?', 'L', ':', 'P', '_',  0,
	 0,   0, '\'',  0,  '{', '+',  0,   0,   0,   0, '\n', '}',  0,  '|',  0,   0,
	 0,   0,   0,   0,   0,   0, '\b',  0,   0,  '1',  0,  '4', '7',  0,   0,   0,
	'0', '.', '2', '5', '6', '8',  0,   0,   0,  '+', '3', '-', '*', '9',  0,   0,
	 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};
