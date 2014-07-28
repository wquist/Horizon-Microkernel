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

/*! \file arch/i586/sw/instr/io.h
 *  \date June 2014
 */

#pragma once

#include <stdint.h>

//! Read a byte of data from an I/O port.
static inline uint8_t inb(uint16_t port)
{
	uint8_t ret;
	__asm ("inb %1, %0" : "=a" (ret) : "d" (port));
	return ret;
}

//! Write a byte of data to an I/O port.
static inline void outb(uint16_t port, uint8_t val)
{
	__asm ("outb %0, %1" :: "a" (val), "d" (port) : "memory");
}
