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

/*! \file arch/i586/boot/meminfo.h
 *  \date June 2014
 */

#pragma once

#include <spec/multiboot.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

 #define MEMINFO_MMAP_MAX 8

//! A section of available memory
typedef struct meminfo_mmap meminfo_mmap_t;
struct meminfo_mmap
{
	uintptr_t start;
	uintptr_t end;
};

void meminfo_init(const multiboot_info_t* mbi);

bool meminfo_region_is_valid(uintptr_t start, size_t size);

size_t meminfo_total_get();
size_t meminfo_usable_get();
uintptr_t meminfo_limit_get();

size_t meminfo_mmap_count_get();
const meminfo_mmap_t* meminfo_mmap_get(size_t index);
