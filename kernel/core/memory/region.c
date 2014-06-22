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

#include "region.h"
#include <arch.h>
#include <debug/log.h>
#include <debug/error.h>
#include <util/addr.h>

static uintptr_t max_mapped = 0;
static uintptr_t next_free  = 0;
static bool idmap_valid = true;

//! Set up the start values for the region allocator.
void region_init(uintptr_t mapped)
{
	max_mapped = mapped;
	// Start marking regions immediately after the kernel ends.
	next_free  = addr_align_next(__ekernel, ARCH_PGSIZE);

	dtrace("First free region at %#X.", next_free);
}

//! Reserve a region after the last, aligned to a page boundary.
/*! Regions can only be allocated; never freed. */
uintptr_t region_reserve(size_t size)
{
	// First align to the next page boundary.
	next_free = addr_align_next(next_free, ARCH_PGSIZE);

	if (idmap_valid) //< Make sure an already mapped region is available.
		dassert(next_free + size < max_mapped);

	uintptr_t curr = next_free;
	next_free += size;

	dtrace("Reserved region from %#X to %#X.", curr, next_free);
	return curr;
}

//! Signal to the allocator that no more ready-mapped regions are needed.
void region_idmap_invalidate()
{
	dassert(idmap_valid);
	dtrace("Ended ID-mapped regions at %#X.", next_free);

	// Skip to the end of the id-mapped region.
	next_free = max_mapped;
	idmap_valid = false;
}

//! Get the next free region available.
uintptr_t region_end_get() { return next_free; }
//! Get the last id-mapped address available.
uintptr_t region_idmap_end_get() { return max_mapped; }
