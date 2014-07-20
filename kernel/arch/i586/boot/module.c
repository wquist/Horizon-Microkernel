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
#include <util/compare.h>
#include <stdio.h>

static size_t mod_count = 0;
static module_t mod_info[MODULE_MAX] = {0};

//! Parse the multiboot information to get relevant module info.
void module_init(const multiboot_info_t* mbi)
{
	if (!(mbi->mods_count))
		return;

	mod_count = min(mbi->mods_count, MODULE_MAX);
	if (mod_count < mbi->mods_count)
		dtrace("Warning: not all modules will be parsed.");

	dtrace("Finding boot modules:");
	multiboot_module_t* mb_mods = (multiboot_module_t*)(mbi->mods_addr);
	for (size_t i = 0; i != mod_count; ++i)
	{
		mod_info[i].start = mb_mods[i].mod_start;
		mod_info[i].size  = mb_mods[i].mod_end - mb_mods[i].mod_start;

		printf("\tAt %#10X.", mod_info[i].start);
		printf("\t(%iKB)\n", mod_info[i].size / 1024);
	}
}

//! Get the number of (parsed) boot modules.
size_t module_count_get()
{
	return mod_count;
}

//! Get a pointer to the specified module.
const module_t* module_get(size_t i)
{
	dassert(i < mod_count);

	return &(mod_info[i]);
}
