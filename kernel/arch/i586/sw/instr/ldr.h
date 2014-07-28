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

/*! \file arch/i586/sw/instr/ldr.h
 *  \date June 2014
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

//! Load a new GDT for the CPU.
static inline void lgdt(const void* gdtr)
{
	__asm ("lgdt (%0)" :: "r" (gdtr) : "memory");
}

//! Load a new IDT for the CPU.
static inline void lidt(const void* idtr)
{
	__asm ("lidt (%0)" :: "r" (idtr) : "memory");
}

//! Load a new task register segment.
static inline void ltr(uint16_t seg, size_t dpl)
{
	uint16_t val = seg | (uint16_t)dpl;
	__asm ("ltr %0" :: "r" (val) : "memory");
}
