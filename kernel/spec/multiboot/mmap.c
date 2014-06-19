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

#include "mmap.h"

//! Get the total memory active in the system.
/*! This includes unusable memory. */
size_t multiboot_mem_size_get(multiboot_info_t* mbi)
{
	if (!(mbi->flags & MULTIBOOT_INFO_MEMORY))
		return 0;

	size_t kilobytes = mbi->mem_upper;
	return (kilobytes + 1024) * 1024; //< Upper memory = total - 1MB.
}

//! Get the next memory map entry in the linked list, or NULL if the end / not available.
/*! Returns the first entry if prev is NULL. */
multiboot_mmap_t* multiboot_mmap_get(multiboot_info_t* mbi, multiboot_mmap_t* prev)
{
	if (!prev)
	{
		if (!(mbi->flags & MULTIBOOT_INFO_MEM_MAP))
			return NULL;

		return (multiboot_mmap_t*)(mbi->mmap_addr);
	}

	uintptr_t addr = (uintptr_t)prev;
	if (addr >= (mbi->mmap_addr + mbi->mmap_length))
		return NULL;

	return (multiboot_mmap_t*)(addr + prev->size + 4);
}
