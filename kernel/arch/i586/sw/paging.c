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

#include "paging.h"
#include <memory/region.h>
#include <debug/error.h>

enum { PAGING_FLAG_PRESENT = 0x1 };

static paging_dir_t* kernel_dir = NULL;
static uintptr_t tempmap_addr = 0;

static inline size_t dir_index(uintptr_t addr) { return (addr >> 22) & 0x3FF; }
static inline size_t tbl_index(uintptr_t addr) { return (addr >> 12) & 0x3FF; }
static inline uint32_t  entry_frame(uintptr_t addr) { return (addr  >> 12); }
static inline uintptr_t entry_addr(uint32_t frame)  { return (frame << 12); }

static inline paging_dirent_t* dirent_find(uintptr_t addr)
{
	return (paging_dirent_t*)(0xFFFFF000 + ((addr >> 22) * 4));
}

static inline paging_tblent_t* tblent_find(uintptr_t addr)
{
	return (paging_tblent_t*)(0xFFC00000 + ((addr >> 12) * 4));
}

void paging_tempmap_init()
{
	tempmap_addr = region_reserve(2 * PAGING_PAGE_SIZE);
}

void paging_init()
{
	dassert(tempmap_addr);

	//
}

paging_dir_t* paging_dir_create()
{
	//

	return NULL;
}

void paging_dir_destroy(paging_dir_t* dir)
{
	//
}

bool paging_map(paging_dir_t* dir, const void* virt, const void* phys, PAGING_FLAGS flags)
{
	//

	return false;
}

void* paging_map_temp(const void* virt)
{
	//

	return NULL;
}

void paging_unmap(paging_dir_t* dir, const void* virt)
{
	//
}

bool paging_is_mapped(const paging_dir_t* dir, const void* virt)
{
	//

	return false;
}

void* paging_mapping_get(const paging_dir_t* dir, const void* virt)
{
	//

	return NULL;
}

paging_dir_t* kernel_pgdir()
{
	return kernel_dir;
}
