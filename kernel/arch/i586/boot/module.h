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

/*! \file arch/i586/boot/module.h
 *  \date July 2014
 */

#pragma once

#include <spec/multiboot.h>
#include <stddef.h>
#include <stdint.h>

//! How many modules will be parsed by the kernel?
/*! Does not prevent more modules from being loaded. */
#define MODULE_MAX 5

typedef struct module module_t;
struct module
{
	uintptr_t start;
	size_t size;
};

void module_init(const multiboot_info_t* mbi);

size_t module_count_get();
const module_t* module_get(size_t index);
