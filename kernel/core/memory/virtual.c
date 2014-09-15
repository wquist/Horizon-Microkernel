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

#include "virtual.h"
#include <memory/physical.h>
#include <multitask/process.h>
#include <debug/error.h>
#include <util/addr.h>
#include <util/compare.h>
#include <horizon/shm.h>
#include <stdbool.h>
#include <memory.h>

static PAGING_FLAGS get_dir(pid_t pid, paging_pas_t** pas)
{
	// The paging functions use NULL to represent the kernel PAS.
	*pas = NULL;
	PAGING_FLAGS flags = PAGING_FLAG_WRITE;

	// PID 0 is the kernel PID, the rest may be invalid.
	process_t* proc = NULL;
	if (pid)
	{
		proc = process_get(pid);
		dassert(proc);
	}

	// If the process exists, the mapping must be for userspace.
	if (proc)
	{
		flags |= PAGING_FLAG_USER;
		*pas = proc->addr_space;
	}

	return flags;
}

//! Map a range of memory to a corresponding region of physical memory.
void virtual_map(pid_t pid, uintptr_t virt, const void* phys, size_t size)
{
	dassert(size);

	paging_pas_t* pas;
	PAGING_FLAGS flags = get_dir(pid, &pas);

	// The physical page must be aligned for paging_map.
	phys = (const void*)addr_align((uintptr_t)phys, ARCH_PGSIZE);

	// Same for the virtual page.
	uintptr_t curr = addr_align(virt, ARCH_PGSIZE);
	// Use virt and not curr because virt+size may extend an extra page over curr+size.
	uintptr_t end  = virt + size;
	for (; curr < end; curr += ARCH_PGSIZE, phys += ARCH_PGSIZE)
		paging_map(pas, curr, phys, flags);
}

//! Remove a range of memory from the PAS.
void virtual_unmap(pid_t pid, uintptr_t virt, size_t size)
{
	dassert(size);

	paging_pas_t* pas;
	get_dir(pid, &pas);

	uintptr_t curr = addr_align(virt, ARCH_PGSIZE);
	uintptr_t end  = virt + size;
	for (; curr < end; curr += ARCH_PGSIZE)
		paging_unmap(pas, curr);
}

//! Check if a region is completely or partially mapped.
/*! Returns 0 if not mapped, 1 if mapped, or -1 if mixed. */
int virtual_is_mapped(pid_t pid, uintptr_t virt, size_t size)
{
	dassert(size);

	paging_pas_t* pas;
	get_dir(pid, &pas);

	uintptr_t curr = addr_align(virt, ARCH_PGSIZE);
	// Use the first page as the basis to check for mixed mappings.
	bool state = paging_is_mapped(pas, curr);

	uintptr_t end  = virt + size;
	for (curr += ARCH_PGSIZE; curr < end; curr += ARCH_PGSIZE)
	{
		bool temp = paging_is_mapped(pas, curr);
		// The rest of the mappings don't matter if it is already mixed.
		if (temp != state)
			return -1;
	}

	return (int)state; //< Either 0 or 1 at this point.
}

//! Map a region with newly allocated physical memory.
/*! Set any allocated memory to 0. */
void virtual_alloc(pid_t pid, uintptr_t virt, size_t size)
{
	dassert(size);

	paging_pas_t* pas;
	PAGING_FLAGS flags = get_dir(pid, &pas);

	uintptr_t curr = addr_align(virt, ARCH_PGSIZE);
	uintptr_t end  = virt + size;
	for (; curr < end; curr += ARCH_PGSIZE)
	{
		// Ignore already mapped pages.
		if (!paging_is_mapped(pas, curr))
		{
			void* block = physical_alloc();
			paging_map(pas, curr, block, flags);
		}

		// FIXME: Use the following as a fallback.
		/* 'block' can just be zero-ed otherwise. */

		// Determine the actual start of allocated memory.
		uintptr_t zero_start = max(curr, virt);
		size_t diff = zero_start - curr;

		// Map the allocated memory into the kernel address space to clear.
		void*  phys = paging_mapping_get(pas, curr) + diff;
		size_t size = min(ARCH_PGSIZE - diff, end - curr);
		memset(paging_map_temp(phys), 0, ARCH_PGSIZE - diff);
	}
}

//! Point virtual memory to the same physical region that is mapped in another process.
//  FIXME: Too many parameters in this function.
void virtual_share(pid_t dest, pid_t src, uintptr_t to, uintptr_t from, size_t size, uint8_t mode)
{
	// No need for get_dir since both processes must exist.
	process_t* pdest = process_get(dest);
	process_t* psrc  = process_get(src);

	dassert(pdest && psrc);
	dassert(size);

	paging_pas_t* pas_dest = pdest->addr_space;
	paging_pas_t* pas_src  = psrc->addr_space;

	PAGING_FLAGS flags = PAGING_FLAG_USER;
	if (mode & SPROT_WRITE)
		flags |= PAGING_FLAG_WRITE;

	uintptr_t curr   = addr_align(from, ARCH_PGSIZE);
	uintptr_t target = addr_align(to, ARCH_PGSIZE);
	uintptr_t end    = from + size;
	for (; curr < end; curr += ARCH_PGSIZE, target += ARCH_PGSIZE)
	{
		// Ignore pages that do not exist in the source.
		if (!paging_is_mapped(pas_src, curr))
			continue;
		// Ignore pages that are mapped in the destination.
		if (paging_is_mapped(pas_dest, target))
			continue;

		void* block = paging_mapping_get(pas_src, curr);
		physical_retain(block); //< One more PAS is referencing the block.

		paging_map(pas_dest, target, block, flags);
	}
}
