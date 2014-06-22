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
#include <arch.h>
#include <sw/instr/info.h>
#include <sw/instr/crx.h>
#include <sw/instr/pg.h>
#include <memory/region.h>
#include <memory/physical.h>
#include <debug/log.h>
#include <debug/error.h>
#include <util/addr.h>
#include <memory.h>

static paging_dir_t* kernel_dir = NULL;
static uintptr_t tempmap_addr = 0;

#define DIR_INDEX(x)  (((uintptr_t)(x) >> 22) & 0x3FF)
#define TBL_INDEX(x)  (((uintptr_t)(x) >> 12) & 0x3FF)
#define ENTRY_FRAME(x) ((uintptr_t)(x) >> 12)
#define ENTRY_ADDR(x)  ((uintptr_t)(x) << 12)

static inline paging_dirent_t* dirent_find(uintptr_t addr)
{
	return (paging_dirent_t*)(0xFFFFF000 + ((addr >> 22) * 4));
}

static inline paging_tblent_t* tblent_find(uintptr_t addr)
{
	return (paging_tblent_t*)(0xFFC00000 + ((addr >> 12) * 4));
}

static paging_dirent_t* dirent_get(const paging_dir_t* dir, uintptr_t virt);
static paging_tblent_t* tblent_get(const paging_dir_t* dir, uintptr_t virt);

//! Reserve the space required for paging_map_temp.
/*! Allocate to pages so structures up to 4KB can be mapped regardless of alignment. */
void paging_tempmap_init()
{
	tempmap_addr = region_reserve(2 * PAGING_PAGE_SIZE);
}

//! Create the main kernel page directory and map important regions to it.
void paging_init()
{
	dassert(tempmap_addr);

	// Try to enable global pages.
	if (cpuid_is_available())
	{
		cpuid_edx_t edx;
		cpuid(CPUID_CODE_FEATURE, (uint32_t*[4]){ [3] = &(edx.raw) });
		if (edx.PGE) //< This processor supports global pages.
		{
			// Enable the actual feature.
			cr4_t cr4 = { .raw = cr4_read() };
			cr4.PGE = true;
			cr4_write(cr4.raw);
		}
	}

	kernel_dir = paging_dir_create();
	paging_dir_t* vdir = paging_map_temp(kernel_dir);

	// Allocate all 3GB+ pgtbls and mark them as global, besides the last (recursive map).
	/* This way, all process page dirs stay in sync for kernel space. */
	for (size_t i = DIR_INDEX(KERNEL_VIRT_BASE); i != PAGING_DIR_TBL_MAX-1; ++i)
	{
		paging_dirent_t* de = &(vdir->entries[i]);
		de->present  = true;
		de->writable = true;
		de->global   = true;

		void* tbl = physical_alloc();
		de->frame = ENTRY_FRAME(tbl);
	}

	// Map low memory (first 1MB) into the space above 3GB before the kernel.
	uintptr_t phys = 0, virt = KERNEL_VIRT_BASE;
	while (virt < KERNEL_VIRT_ADDR)
	{
		paging_map(kernel_dir, virt, (void*)phys, PAGING_FLAG_WRITE);

		phys += PAGING_PAGE_SIZE;
		virt += PAGING_PAGE_SIZE;
	}

	// Map the kernel into its higher memory space (3GB + 1MB).
	phys = KERNEL_PHYS_ADDR; virt = KERNEL_VIRT_ADDR;
	while (virt < region_idmap_end_get())
	{
		paging_map(kernel_dir, virt, (void*)phys, PAGING_FLAG_WRITE);

		phys += PAGING_PAGE_SIZE;
		virt += PAGING_PAGE_SIZE;
	}

	paging_dir_load(kernel_dir);
	dtrace("Switched to main kernel page directory.");
}

//! Create a new page directory, and map kernel space into it if available.
paging_dir_t* paging_dir_create()
{
	void* dir = physical_alloc();
	paging_dir_t* vdir = paging_map_temp(dir);
	memset(vdir, 0, sizeof(paging_dir_t));

	// Set up recursive mapping.
	paging_dirent_t* de_last = &(vdir->entries[PAGING_DIR_TBL_MAX-1]);
	de_last->present  = true;
	de_last->writable = true;
	de_last->frame    = ENTRY_FRAME(dir);

	// If the kernel page directory has been created, map in 3GB+ from it to the new one.
	if (kernel_dir)
	{
		for (size_t i = DIR_INDEX(KERNEL_VIRT_BASE); i != PAGING_DIR_TBL_MAX-1; ++i)
		{
			/* Both kdir and tdir need to be temp mapped, so map kdir first, copy
			 * the target dirent to the stack, then map tdir and set the dirent.
			 */
			volatile paging_dir_t* kdir = paging_map_temp(kernel_dir);
			paging_dirent_t kde = kdir->entries[i];

			volatile paging_dir_t* tdir = paging_map_temp(dir);
			tdir->entries[i] = kde;
		}
	}

	dtrace("Allocated new page directory at %p.", dir);
	return (paging_dir_t*)dir;
}

//! Destroy a page directory, returning all used memory to the physical allocator.
void paging_dir_destroy(paging_dir_t* dir)
{
	dassert(dir && dir != kernel_dir);

	for (size_t i = 0; i != DIR_INDEX(KERNEL_VIRT_BASE); ++i) //< Only free 0-3GB.
	{
		volatile paging_dir_t* vdir = paging_map_temp(dir);

		paging_dirent_t* de = (paging_dirent_t*)&(vdir->entries[i]);
		if (!(de->present))
			continue;

		void* tbl = (void*)ENTRY_ADDR(de->frame);
		paging_tbl_t* vtbl = paging_map_temp(tbl);

		// Free the entire range of page mappings in the table.
		for (size_t j = 0; j != PAGING_TBL_PAGE_MAX; ++j)
		{
			paging_tblent_t* te = &(vtbl->entries[i]);
			if (!(te->present))
				continue;

			void* block = (void*)ENTRY_ADDR(te->frame);
			// The release will just be ignored if 'block' was not actually allocated.
			physical_release(block);
		}

		physical_release(tbl);
	}

	dtrace("Freed page directory at %p.", dir);
	physical_release(dir);
}

//! Map a page to a physical address with the given flags.
/*! All addresses passed should be aligned. */
bool paging_map(paging_dir_t* dir, uintptr_t virt, const void* phys, PAGING_FLAGS flags)
{
	dassert(addr_align(virt, PAGING_PAGE_SIZE) == virt);
	dassert(addr_align((uintptr_t)phys, PAGING_PAGE_SIZE) == (uintptr_t)phys);

	// Pass NULL to map with the kernel page directory.
	if (!dir) dir = kernel_dir;
	dassert(dir);

	paging_tbl_t* vtbl = NULL;

	// First make sure the corresponding page table is set up.
	paging_dirent_t* de = dirent_get(dir, virt);
	if (de->present)
	{
		// Update the dirent flags incase this mapping needs different privs.
		de->writable |= !!(flags & PAGING_FLAG_WRITE);
		de->user     |= !!(flags & PAGING_FLAG_USER);

		// The page table already exists so map it in.
		void* frame = (void*)ENTRY_ADDR(de->frame);
		vtbl = paging_map_temp(frame);
	}
	else
	{
		// Set up the default flags for the (unitialized) dirent.
		memset(de, 0, sizeof(paging_dirent_t));
		de->present  = true;
		de->writable = !!(flags & PAGING_FLAG_WRITE);
		de->user     = !!(flags & PAGING_FLAG_USER);

		// Need to create the page table here.
		void* tbl = physical_alloc();
		de->frame = ENTRY_FRAME(tbl);

		vtbl = paging_map_temp(tbl);
		memset(vtbl, 0, sizeof(paging_tbl_t));

		if (paging_dir_is_active(dir))
		{
			// Invalidate 1024 pages (the table's pages) or just reload CR3.
			// Also invalidates the pointer to the recursively mapped group of PTEs.
			paging_dir_load(dir);
		}
	}

	// Next, if the PTE is not mapped or the map is forced, update it.
	paging_tblent_t* te = &(vtbl->entries[TBL_INDEX(virt)]);
	if (!(te->present) || (flags & PAGING_FLAG_FORCE))
	{
		memset(te, 0, sizeof(paging_tblent_t));
		te->present  = true;
		te->writable = !!(flags & PAGING_FLAG_WRITE);
		te->user     = !!(flags & PAGING_FLAG_USER);
		te->frame    = ENTRY_FRAME(phys);

		if (paging_dir_is_active(dir))
			invlpg(virt); //< Invalidate the single page affected.

		return true;
	}

	return false;
}

//! Map physical memory to a predetermined volatile address in the current pgdir.
/*! Tempmap space is 8KB, so 2 page maps are needed. */
void* paging_map_temp(const void* phys)
{
	dassert(tempmap_addr);

	paging_tblent_t* te_low = tblent_find(tempmap_addr);
	te_low->frame = ENTRY_FRAME(phys);
	invlpg(tempmap_addr);

	// The start of the second half of tempmap space is 4KB above the first.
	paging_tblent_t* te_high = tblent_find(tempmap_addr + PAGING_PAGE_SIZE);
	te_high->frame = ENTRY_FRAME(phys + PAGING_PAGE_SIZE);
	invlpg(tempmap_addr + PAGING_PAGE_SIZE);

	// Calculate the proper offset if phys was not page aligned.
	return (void*)(tempmap_addr + ((uintptr_t)phys % PAGING_PAGE_SIZE));
}

//! Set a page as not-present.
void paging_unmap(paging_dir_t* dir, uintptr_t virt)
{
	dassert(addr_align(virt, PAGING_PAGE_SIZE) == virt);

	if (!dir) dir = kernel_dir;
	dassert(dir);

	paging_tblent_t* te = tblent_get(dir, virt);
	if (!te || !(te->present)) return; //< Already unmapped.

	// The memory for the page mapping can be released.
	void* block_frame = (void*)ENTRY_ADDR(te->frame);
	physical_release(block_frame);

	// The page is no longer present, invalidate it.
	te->present = false;
	invlpg(virt);
}

//! Check if a page is present.
bool paging_is_mapped(const paging_dir_t* dir, uintptr_t virt)
{
	dassert(addr_align(virt, PAGING_PAGE_SIZE) == virt);

	if (!dir) dir = kernel_dir;
	dassert(dir);

	paging_tblent_t* te = tblent_get(dir, virt);	
	return (te && te->present);
}

//! Get the physical mapping of a page.
void* paging_mapping_get(const paging_dir_t* dir, uintptr_t virt)
{
	dassert(addr_align(virt, PAGING_PAGE_SIZE) == virt);

	if (!dir) dir = kernel_dir;
	dassert(dir);

	paging_tblent_t* te = tblent_get(dir, virt);
	if (!te || !(te->present)) return NULL;

	return (void*)ENTRY_ADDR(te->frame);
}

//! Get a pointer to the main kernel page directory.
paging_dir_t* kernel_pgdir()
{
	return kernel_dir;
}

//! Get a pointer to the specified PDE.
paging_dirent_t* dirent_get(const paging_dir_t* dir, uintptr_t virt)
{
	paging_dir_t* vdir = paging_map_temp(dir);
	return &(vdir->entries[DIR_INDEX(virt)]);
}

//! Get a pointer to the given PTE.
/*! Returns NULL if the corresponding pgtbl does not exist. */
paging_tblent_t* tblent_get(const paging_dir_t* dir, uintptr_t virt)
{
	// First get the PDE.
	paging_dirent_t* de = dirent_get(dir, virt);
	if (!(de->present)) return NULL; // PTE can't exist without a PDE.

	// Now map in the page table.
	void* frame = (void*)ENTRY_ADDR(de->frame);
	paging_tbl_t* vtbl = paging_map_temp(frame);

	// Return the PTE; may or may not be present.
	return &(vtbl->entries[TBL_INDEX(virt)]);
}
