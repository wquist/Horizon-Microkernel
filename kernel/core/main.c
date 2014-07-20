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

/*! \file core/main.c
 *  \date May 2014
 *
 *  /mainpage Horizon Microkernel
 *  /version  0.0.5-0
 */

#include <arch.h>
#include <memory/region.h>
#include <memory/physical.h>
#include <multitask/process.h>
#include <multitask/scheduler.h>
#include <debug/ctl.h>
#include <debug/log.h>
#include <debug/error.h>
#include <spec/elf.h>
#include <stdint.h>
#include <stdio.h>

void _Noreturn kmain(int magic, const bootloader_info_t* bli, uintptr_t vmem)
{
	debug_init();
	dassert(magic == BOOTLOADER_MAGIC);

	// Initialize anything platform-dependent.
	arch_init();

	// Initialize arch-specific memory info.
	region_init(vmem);
	meminfo_init(bli);
	module_init(bli);
	paging_tempmap_init();

	// Initial physical and virtual memory management.
	physical_init();
	region_idmap_invalidate();
	paging_init();

	// Initialize multitasking.
	int_init();
	process_init();

	// Load available modules as processes.
	for (size_t i = 0; i != module_count_get(); ++i)
	{
		const module_t* mod = module_get(i);
		if (elf_module_validate(mod))
		{
			elf_binary_t binary = elf_module_parse(mod);
			uint16_t pid = elf_binary_load(&binary);

			uint16_t tid = thread_new(pid, 0);
			scheduler_add(tid);
		}
	}

	// Begin the first usermode thread.
	scheduler_start();

	// Should never be reached.
	dtrace("Unexpected end of kernel reached!");
	for (;;);
}
