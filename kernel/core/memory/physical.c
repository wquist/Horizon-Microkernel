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

#include "physical.h"
#include <arch.h>
#include <memory/region.h>
#include <util/addr.h>
#include <util/bmstack.h>
#include <limits.h>
#include <memory.h>

static bmstack_t alloc_map = {0};
static uint8_t* ref_counts = NULL;

//! Set up the bitmap stack as well as reference counting.
void physical_init()
{
	size_t blocks = addr_to_index(PHYSICAL_USABLE_BASE, ARCH_PGSIZE, meminfo_limit_get());

	size_t alloc_size = BMSTACK_SIZE(blocks);
	uintptr_t alloc_start = region_reserve(alloc_size);
	dtrace("Allocated physical memory bitmap. (%iKB)", alloc_size / 1024);

	bmstack_init(&alloc_map, (void*)alloc_start);
	bmstack_set_all(&alloc_map, blocks); //< Mark everything as used first.

	// Unmark any available memory.
	for (size_t i = 0; i != meminfo_mmap_count_get(); ++i)
	{
		// All available memory ranges are stored in the meminfo mmaps.
		const meminfo_mmap_t* mmap = meminfo_mmap_get(i);
		for (uintptr_t curr = mmap->start; curr + ARCH_PGSIZE <= mmap->end; curr += ARCH_PGSIZE)
		{
			size_t index = addr_to_index(PHYSICAL_USABLE_BASE, ARCH_PGSIZE, curr);
			bmstack_clear(&alloc_map, index);
		}
	}

	// Create the linked list from the entries just unset.
	bmstack_link(&alloc_map, blocks);

	size_t ref_size = blocks * sizeof(uint8_t);
	ref_counts = (uint8_t*)region_reserve(ref_size);
	dtrace("Allocated physical memory refcount array. (%iKB)", ref_size / 1024);

	memset(ref_counts, 0x00, blocks);
}

//! Allocate a new block of memory (equal to platform page size).
void* physical_alloc()
{
	dassert(ref_counts);

	size_t index = bmstack_find_and_set(&alloc_map);
	dassert(index != -1);

	ref_counts[index] = 1;
	uintptr_t addr = index_to_addr(PHYSICAL_USABLE_BASE, ARCH_PGSIZE, index);

	return (void*)addr;
}

//! Increment the retain count for the given block. Block must be from physical_alloc.
void physical_retain(const void* block)
{
	dassert(ref_counts);

	size_t index = addr_to_index(PHYSICAL_USABLE_BASE, ARCH_PGSIZE, (uintptr_t)block);
	if (!(ref_counts[index])) //< Bad block; it was not previously allocated.
		return;

	dassert(ref_counts[index] < UCHAR_MAX);
	++(ref_counts[index]);
}

//! Decrement the retain count for the given block and free it if is no longer retained.
/*! Returns true if the released block was freed, false otherwise. */
bool physical_release(void* block)
{
	dassert(ref_counts);

	size_t index = addr_to_index(PHYSICAL_USABLE_BASE, ARCH_PGSIZE, (uintptr_t)block);
	if (!(ref_counts[index]))
		return false;

	--(ref_counts[index]);
	if (ref_counts[index]) //< This block is still retained.
		return false;

	dassert(bmstack_test(&alloc_map, index));
	bmstack_clear(&alloc_map, index); //< 'free' it by making available for allocation.

	return true;
}
