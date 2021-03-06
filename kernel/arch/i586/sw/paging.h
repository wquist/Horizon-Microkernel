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

/*! \file arch/i586/sw/paging.h
 *  \date June 2014
 */

#pragma once

#include <sw/instr/crx.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define PAGING_PAGE_SIZE 4096

#define PAGING_TBL_PAGE_MAX 1024
#define PAGING_DIR_TBL_MAX  1024

typedef enum paging_flags PAGING_FLAGS;
enum paging_flags
{
	PAGING_FLAG_WRITE   = 0x1, //< Read-Write access.
	PAGING_FLAG_USER    = 0x2, //< Ring-3 access.

	PAGING_FLAG_FORCE   = 0x4  //< Overwrite the previous mapping, if it exists.
};

//! A page directory entry. (PDE)
typedef union paging_dirent paging_dirent_t;
union paging_dirent
{
	struct __packed
	{
		uint32_t present   : 1;
		uint32_t writable  : 1;
		uint32_t user      : 1;
		uint32_t writethru : 1; //!< Write-through cache mode.
		uint32_t nocache   : 1; //!< Prevent adress from being cached.
		uint32_t accessed  : 1; //!< Set to 1 on read/write. Never auto-reset to 0.
		uint32_t           : 1;
		uint32_t big       : 1; //!< Point to a 4MB range instead of a pgtbl.
		uint32_t global    : 1; //!< Ignored for PDEs...
		uint32_t available : 3;
		uint32_t frame     : 20;
	};
	uint32_t raw;
};

//! A page table entry. (PTE)
typedef union paging_tblent paging_tblent_t;
union paging_tblent
{
	struct __packed
	{
		uint32_t present    : 1;
		uint32_t writable   : 1;
		uint32_t user       : 1;
		uint32_t writethru  : 1;
		uint32_t nocache    : 1;
		uint32_t accessed   : 1;
		uint32_t dirty      : 1; //!< Set to 1 on write. Never auto-resets.
		uint32_t attr_index : 1; //!< Always 0. (?)
		uint32_t global     : 1; //!< This cache entry is not reset on CR3 reload.
		uint32_t  available : 3;
		uint32_t frame      : 20;
	};
	uint32_t raw;
};

//! A page directory. Holds PDEs.
typedef struct paging_dir paging_dir_t;
struct paging_dir 
{
	paging_dirent_t entries[PAGING_DIR_TBL_MAX];
};

//! A page table. Holds PTEs.
typedef struct paging_tbl paging_tbl_t;
struct paging_tbl
{
	paging_tblent_t entries[PAGING_TBL_PAGE_MAX];
};

//! Set the active page directory.
static inline void paging_dir_load(const paging_dir_t* dir) { cr3_write((uint32_t)dir); }

//! Compare the current directory with the active one.
static inline bool paging_dir_is_active(const paging_dir_t* dir)
{
	// The bottom bits may contain flags so use the structs for an easy compare.
	cr3_t curr = { .raw = cr3_read() };
	cr3_t this = { .raw = (uint32_t)dir };

	return (curr.PDBR == this.PDBR);
}

void paging_tempmap_init();
void paging_init();

paging_dir_t* paging_dir_create();
void paging_dir_destroy(paging_dir_t* dir);

bool  paging_map(paging_dir_t* dir, uintptr_t virt, const void* phys, PAGING_FLAGS flags);
void* paging_map_temp(const void* phys);
void  paging_unmap(paging_dir_t* dir, uintptr_t virt);

bool  paging_is_mapped(const paging_dir_t* dir, uintptr_t virt);
void* paging_mapping_get(const paging_dir_t* dir, uintptr_t virt);

paging_dir_t* kernel_pgdir();
