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

#include "process.h"
#include <memory/region.h>
#include <memory/virtual.h>
#include <util/bmstack.h>
#include <util/addr.h>
#include <debug/log.h>
#include <debug/error.h>
#include <horizon/priv.h>
#include <memory.h>

static process_t* blocks = NULL;
static bmstack_t  block_map = {0};

//! Initialize the process bitmap and call thread_init.
void process_init()
{
	dassert(sizeof(process_t) <= PROCESS_BLOCK_SIZE);

	// Reserve space for PCBs, but do not map them to physical memory yet.
	/* PCBs are allocated as they are needed to save memory. */
	blocks = (process_t*)region_reserve(PROCESS_MAX * PROCESS_BLOCK_SIZE);

	// Make space for the bitmap, and map the entire space to physical memory.
	uintptr_t map_start = region_reserve(BMSTACK_SIZE(PROCESS_MAX));
	bmstack_init(&block_map, (void*)map_start);
	virtual_alloc(0, map_start, BMSTACK_SIZE(PROCESS_MAX));

	// Reserve some special PIDs (self, kernel, any).
	bmstack_set(&block_map, 0);
	bmstack_set(&block_map, 1);
	bmstack_set(&block_map, PROCESS_MAX-1);

	bmstack_link(&block_map, PROCESS_MAX);
	dtrace("Initialized process bitmap. (%iKB)", BMSTACK_SIZE(PROCESS_MAX)/1024);

	thread_init();
}

//! Create a new process with the given parent and default entry point.
/*! The parent determines the new process's privilege. */
pid_t process_new(pid_t ppid, uintptr_t entry)
{
	size_t index = bmstack_find_and_set(&block_map);
	dassert(index != -1); //< No PID available.

	// Get the address of the PCB and map it in if necessary.
	uintptr_t block = index_to_addr((uintptr_t)blocks, PROCESS_BLOCK_SIZE, index);
	virtual_alloc(0, block, PROCESS_BLOCK_SIZE);

	process_t* process = (process_t*)block;
	memset(process, 0, PROCESS_BLOCK_SIZE);

	process_t* parent = process_get(ppid);
	process->pid    = index;
	process->parent = (parent) ? ppid : 0;
	process->priv   = (parent) ? parent->priv : PRIV_DRIVER;

	process->entry = entry;
	process->addr_space = paging_pas_create();

	dtrace("Created process with PID %i.", index);
	return (pid_t)index;
}

//! Destroy a process, along with its address space.
void process_kill(pid_t pid)
{
	process_t* target = process_get(pid);
	dassert(target);

	// Set the PCB as free but do not free the physical memory.
	bmstack_clear(&block_map, pid);
	paging_pas_destroy(target->addr_space);

	// FIXME: Add bitmap function to find first set bit.
	for (size_t i = 0; i != PROCESS_THREAD_MAX; ++i)
	{
		if (bitmap_test(target->threads.bitmap, i))
			thread_kill(target->threads.slots[i]);
	}

	dtrace("Destroyed process with PID %i.", pid);
}

//! Get the PCB for the given PID.
process_t* process_get(pid_t pid)
{
	dassert(pid < PROCESS_MAX);

	// Check for reserved PIDs.
	if (pid == 0 || pid == 1 || pid == PROCESS_MAX-1)
		return NULL;

	if (!bmstack_test(&block_map, pid))
		return NULL;
	else
		return (process_t*)index_to_addr((uintptr_t)blocks, PROCESS_BLOCK_SIZE, pid);
}
