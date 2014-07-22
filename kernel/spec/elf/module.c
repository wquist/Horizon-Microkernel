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

#include "module.h"
#include <debug/log.h>
#include <debug/error.h>
#include <util/addr.h>
#include <util/compare.h>
#include <limits.h>
#include <string.h>

//! Read the potential elf header to determine if a module is valid.
/*! Also checks architecture and word size. */
bool elf_module_validate(const module_t* module)
{
	dassert(module);

	elf_file_header_t* fheader = paging_map_temp((void*)(module->start));

	char* fmagic = (char*)&(fheader->fid[ELF_FID_MAG]);
	if (memcmp("\x7F""ELF", fmagic, ELF_FID_MAG_LENGTH) != 0)
		return false;

	// Word length cannot be bigger than arch bits.
	switch (fheader->fid[ELF_FID_CLASS])
	{
		case ELF_CLASS_32:
		{
			if (LONG_BIT != 32)
				return false;
			break;
		}
		case ELF_CLASS_64: //< FIXME: Should 64-bit be required on  64-bit?
		{
			if (LONG_BIT == 32)
				return false;
			break;
		}
		default: return false;
	}

// FIXME: Must be a better way than macro checks.
#if defined(ARCH_X86)
	if (fheader->arch != ELF_ARCH_386)
		return false;
#endif

	return true;
}

//! Parse the relevant file and program header info of an .ELF.
elf_binary_t elf_module_parse(const module_t* module)
{
	dassert(module);
	dassert(elf_module_validate(module));

	elf_binary_t binary = {0};

	// Modules are not mapped to virtual memory, so temp map it.
	elf_file_header_t* fheader_phys = (elf_file_header_t*)(module->start);
	elf_file_header_t* fheader = paging_map_temp((void*)fheader_phys);

	// The location of the _start symbol.
	binary.entry = fheader->entry;

	dassert(fheader->pheader_offset); //< Make sure program header info exists.
	uintptr_t pheader_start = (uintptr_t)fheader_phys + fheader->pheader_offset;

	// Define count early because temp mapping pheaders invalidates 'fheader'.
	size_t count = min(fheader->pheader_count, ELF_SECTION_MAX);
	if (count < fheader->pheader_count)
		dtrace("Warning: not all program headers will be parsed.");

	for (size_t i = 0; i != fheader->pheader_count; ++i)
	{
		uintptr_t pheader_phys = index_to_addr(pheader_start, sizeof(elf_program_header_t), i);
		elf_program_header_t* pheader = paging_map_temp((void*)pheader_phys);

		if (pheader->type != ELF_PTYPE_LOAD)
			continue;

		binary.sections[i].phys = (uintptr_t)fheader_phys + pheader->offset;
		binary.sections[i].virt = pheader->vaddr;

		binary.sections[i].size    = pheader->file_size;
		binary.sections[i].reserve = pheader->mem_size;

		binary.sections[i].flags = pheader->flags;
	}

	return binary;
}
