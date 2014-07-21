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
#include <stdbool.h>

static paging_pas_t* get_dir(uint16_t pid, PAGING_FLAGS* flags)
{
	if (flags)
		*flags = PAGING_FLAG_WRITE;

	// PID 0 is the kernel PID, the rest may be invalid.
	process_t* proc = NULL;
	if (pid)
	{
		proc = process_get(pid);
		dassert(proc);
	}

	if (proc)
	{
		if (flags) //< If the process exists, the mapping must be for userspace.
			*flags |= PAGING_FLAG_USER;

		return proc->addr_space;
	}

	// The paging functions use NULL to represent the kernel PAS.
	return NULL;
}

//! Map a range of memory to a corresponding region of physical memory.
void virtual_map(uint16_t pid, uintptr_t virt, const void* phys, size_t size)
{
	dassert(size);

	PAGING_FLAGS flags;
	paging_pas_t* pas = get_dir(pid, &flags);
	phys = (const void*)addr_align((uintptr_t)phys, ARCH_PGSIZE);

	// The paging functions expect aligned addresses.
	uintptr_t curr = addr_align(virt, ARCH_PGSIZE);
	// Use virt and not curr because virt+size may extend an extra page over curr+size.
	uintptr_t end  = virt + size;
	for (; curr < end; curr += ARCH_PGSIZE, phys += ARCH_PGSIZE)
		paging_map(pas, curr, phys, flags);
}

//! Remove a range of memory from the PAS.
void virtual_unmap(uint16_t pid, uintptr_t virt, size_t size)
{
	dassert(size);

	paging_pas_t* pas = get_dir(pid, NULL);

	uintptr_t curr = addr_align(virt, ARCH_PGSIZE);
	uintptr_t end  = virt + size;
	for (; curr < end; curr += ARCH_PGSIZE)
		paging_unmap(pas, curr);
}

//! Check if a region is completely or partially mapped.
/*! Returns 0 if not mapped, 1 if mapped, or -1 if mixed. */
int virtual_is_mapped(uint16_t pid, uintptr_t virt, size_t size)
{
	dassert(size);

	paging_pas_t* pas = get_dir(pid, NULL);
	uintptr_t curr = addr_align(virt, ARCH_PGSIZE);

	// Use the first page as the basis to check for mixed mappings.
	bool state = paging_is_mapped(pas, curr);
	curr += ARCH_PGSIZE;

	uintptr_t end  = virt + size;
	for (; curr < end; curr += ARCH_PGSIZE)
	{
		bool temp = paging_is_mapped(pas, curr);
		// The rest of the mappings don't matter if it is already mixed.
		if (temp != state)
			return -1;
	}

	return (int)state; //< Either 0 or 1 at this point.
}

//! Map a region with newly allocated physical memory.
void virtual_alloc(uint16_t pid, uintptr_t virt, size_t size)
{
	dassert(size);

	PAGING_FLAGS flags;
	paging_pas_t* pas = get_dir(pid, &flags);

	uintptr_t curr = addr_align(virt, ARCH_PGSIZE);
	uintptr_t end  = virt + size;
	for (; curr < end; curr += ARCH_PGSIZE)
	{
		// Ignore already mapped pages.
		if (paging_is_mapped(pas, curr))
			continue;

		/* FIXME: Instead of checking if the page is mapped, check if the result
		 * of paging_map, and if false reuse the block for the next loop iter or
		 * free it and move on. Since ignoring will rarely occur, may be more
		 * efficient that way than to check each block?
		 */
		void* block = physical_alloc();
		paging_map(pas, curr, block, flags);
	}
}

//! Point virtual memory to the same physical region that is mapped in another process.
void virtual_clone(uint16_t dest, uint16_t src, uintptr_t from, uintptr_t to, size_t size, VIRTUAL_CLONE_MODE mode)
{
	// No need for get_dir since both processes must exist.
	process_t* pdest = process_get(dest);
	process_t* psrc  = process_get(src);

	dassert(pdest && psrc);
	dassert(size);

	paging_pas_t* pas_dest = pdest->addr_space;
	paging_pas_t* pas_src  = psrc->addr_space;

	PAGING_FLAGS flags = PAGING_FLAG_USER;
	switch (mode)
	{
		case VIRTUAL_CLONE_READ: 
			break;
		case VIRTUAL_CLONE_WRITE:
			flags |= PAGING_FLAG_WRITE;
			break;
		default: dpanic("Invalid VIRTUAL_CLONE mode.");
	}

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
