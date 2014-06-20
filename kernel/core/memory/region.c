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

//! FIXME: temporary define until arch paging is written.
#define ARCH_PGSIZE 4096

static uintptr_t max_mapped = 0;
static uintptr_t next_free  = 0;
static bool idmap_valid = true;

void region_init(uintptr_t mapped)
{
	max_mapped = mapped;
	next_free  = addr_align_next(__ekernel, ARCH_PGSIZE);

	dtrace("First free region at %#X.", next_free);
}

uintptr_t region_reserve(size_t size, bool aligned)
{
	if (aligned)
		next_free = addr_align_next(next_free, ARCH_PGSIZE);

	if (idmap_valid)
		dassert(next_free + size < max_mapped);

	uintptr_t curr = next_free;
	next_free += size;

	dtrace("Reserved region from %#X to %#X.", curr, next_free);
	return curr;
}

void region_idmap_invalidate()
{
	dassert(idmap_valid);

	max_mapped  = next_free;
	idmap_valid = false;

	dtrace("Ended ID-mapped regions at %#X.", max_mapped);
}

uintptr_t region_end_get() { return next_free; }
uintptr_t region_idmap_end_get() { return max_mapped; }
