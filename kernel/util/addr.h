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

/*! \file util/addr.h
 *  \date June 2014
 */

#pragma once

#include <debug/error.h>
#include <stddef.h>
#include <stdint.h>

//! Align down an address to the given boundary.
static inline uintptr_t addr_align(uintptr_t addr, size_t bound)
{
	dassert(bound && (bound & (bound-1)) == 0); //< Make sure bound is a power of two.
	return (addr & ~(bound-1));
}

//! Align up an address to the given boundary.
static inline uintptr_t addr_align_next(uintptr_t addr, size_t bound)
{
	return addr_align(addr + bound-1, bound);
}

//! Convert a physical address to an index based on the given offset and element size.
static inline size_t addr_to_index(uintptr_t base, size_t blocksize, uintptr_t addr)
{
	return ((addr - base) / blocksize);
}

//! Convert an index to a physical address.
static inline uintptr_t index_to_addr(uintptr_t base, size_t blocksize, size_t index)
{
	return (base + (index * blocksize));
}
