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

/*! \file spec/elf/module.h
 *  \date July 2014
 */

#pragma once

#include <spec/elf.h>
#include <stddef.h>
#include <stdint.h>

//! Abstracted section info from a program header.
typedef struct elf_section elf_section_t;
struct elf_section
{
	uintptr_t phys, virt;
	size_t size, reserve;
	uint32_t flags;
};

//! Abstracted info for the entire .elf file.
typedef struct elf_binary elf_binary_t;
struct elf_binary
{
	elf_section_t sections[ELF_SECTION_MAX];
	uintptr_t entry;
};

bool elf_module_validate(const module_t* module);
elf_binary_t elf_module_parse(const module_t* module);
