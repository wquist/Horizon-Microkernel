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
#include <memory/virtual.h>
#include <multitask/process.h>
#include <multitask/scheduler.h>
#include <system/syscalls.h>
#include <debug/ctl.h>
#include <debug/error.h>
#include <spec/elf/module.h>
#include <util/addr.h>
#include <stdint.h>
#include <stdio.h>

static uint16_t startp_load(const module_t* mod);

// The kernel C entry point.
void __noreturn kmain(int magic, const bootloader_info_t* bli, uintptr_t vmem)
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
	syscall_init();

	// Load available modules as processes.
	for (size_t i = 0; i != module_count_get(); ++i)
	{
		uint16_t pid = startp_load(module_get(i));
		if (!pid)
			continue;

		uint16_t tid = thread_new(pid, 0);
		scheduler_add(tid);
	}

	// Begin the first usermode thread.
	scheduler_start();

	// scheduler_start should never return.
	dpanic("Unexpected end of kernel reached!");
}

// Load a startup process and return its PID.
uint16_t startp_load(const module_t* mod)
{
	if (!elf_module_validate(mod))
		return 0;

	elf_binary_t binary = elf_module_parse(mod);

	uint16_t pid = process_new(0, binary.entry);
	for (size_t i = 0; i != ELF_SECTION_MAX; ++i)
	{
		elf_section_t* sect = &(binary.sections[i]);
		// The section may not have been loadable.
		/* FIXME: Could have a 'loaded' flag instead of phys == 0. */
		if (!(sect->phys))
			continue;

		// Make sure addresses are properly aligned for the arch.
		uintptr_t phys = addr_align(sect->phys, ARCH_PGSIZE);
		uintptr_t virt = addr_align(sect->virt, ARCH_PGSIZE);
		dassert(virt + sect->reserve <= KERNEL_VIRT_BASE);

		// First map the actual data from physical memory.
		virtual_map(pid, virt, (void*)phys, sect->size);

		// Then map any remaining with new memory.
		ssize_t rem_size = sect->reserve - sect->size;
		if (rem_size)
			virtual_alloc(pid, virt + sect->size, rem_size);
	}

	return pid;
}
