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

#include "meminfo.h"
#include <debug/log.h>
#include <debug/error.h>
#include <spec/multiboot/mmap.h>
#include <util/compare.h>
#include <limits.h>

static size_t mem_total    = 0;
static size_t mem_usable   = 0;
static uintptr_t mem_limit = 0;

static size_t mmap_count = 0;
static meminfo_mmap_t mmaps[MEMINFO_MMAP_MAX] = {0};

//! Read the memory and mmap information from the multiboot structure.
void meminfo_init(const multiboot_info_t* mbi)
{
	mem_total = multiboot_mem_size_get(mbi);
	dassert(mem_total != 0);
	dtrace("Total memory: %iKB. Memory map: ", mem_total / 1024);

	const multiboot_mmap_t* entry = multiboot_mmap_get(mbi, NULL);
	for (; entry; entry = multiboot_mmap_get(mbi, entry))
	{
		// Check for overflow - undefined region if so (for 32-bit at least).
		if (entry->addr > ULONG_MAX - entry->length)
			continue;

		uintptr_t end = entry->addr + entry->length;
		dprintf("\t[%i] ", entry->type);
		dprintf("%#10X - %#10X\n", entry->addr, end);

		// Otherwise this memory is reserved or damaged.
		if (entry->type != MULTIBOOT_MEMORY_AVAILABLE)
			continue;

		mem_usable += entry->length;
		mem_limit = max(mem_limit, end);

		dassert(mmap_count < MEMINFO_MMAP_MAX);
		mmaps[mmap_count].start = entry->addr;
		mmaps[mmap_count].end   = end;

		++mmap_count;
	}
}

//! Check if the given range lies completely in available memory.
/*! This assumes each mmap entry is not adjacent; entries should not be fragmented. */
bool meminfo_region_is_valid(uintptr_t start, size_t size)
{
	// Check for overflow.
	dassert(start <= ULONG_MAX - size);

	uintptr_t end = start + size;
	for (size_t i = 0; i != mmap_count; ++i)
	{
		if (mmaps[i].start <= start && mmaps[i].end > end)
			return true;
	}

	return false;
}

//! Get the combined size of all types of memory available.
size_t meminfo_total_get() { return mem_total; }
//! Get the sum of all usable regions of memory.
size_t meminfo_usable_get() { return mem_usable; }
//! Get the highest usable memory address.
uintptr_t meminfo_limit_get() { return mem_limit; }

//! Get the number of available memory regions.
size_t meminfo_mmap_count_get() { return mmap_count; }

//! Returns the mmap range info for the given index.
const meminfo_mmap_t* meminfo_mmap_get(size_t index)
{
	dassert(index < mmap_count);
	return &(mmaps[index]);
}
