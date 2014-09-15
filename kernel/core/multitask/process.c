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
#include <util/compare.h>
#include <util/addr.h>
#include <util/bmstack.h>
#include <debug/log.h>
#include <debug/error.h>
#include <horizon/ipc.h>
#include <horizon/priv.h>
#include <memory.h>

typedef struct block_data block_data_t;
struct block_data
{
	process_t process;
	message_t messages[PROCESS_MESSAGE_MAX];
	thread_t  threads[PROCESS_THREAD_MAX];
};

static block_data_t* blocks    = NULL;
static bmstack_t     block_map = {0};

static uint8_t block_versions[PROCESS_MAX] = {0};

//! Reserve space for the process control blocks and setup management structures.
void process_init()
{
	// Reserve space for the PCBs, but do not allocate memory.
	blocks = (block_data_t*)region_reserve(PROCESS_MAX * sizeof(block_data_t));

	// Make space for the bitmap, and actually map this memory.
	uintptr_t map_start = region_reserve(BMSTACK_SIZE(PROCESS_MAX));
	bmstack_init(&block_map, (void*)map_start);
	virtual_alloc(0, map_start, BMSTACK_SIZE(PROCESS_MAX));

	// Reserve the special PIDs.
	bmstack_set(&block_map, 0);
	bmstack_set(&block_map, 1);

	bmstack_link(&block_map, PROCESS_MAX);
	dtrace("Initialize process control blocks and bitmap. (%iKB)", BMSTACK_SIZE(PROCESS_MAX)/1024);
}

//! Allocate a new process control block with the given parent ID.
pid_t process_new(pid_t ppid, uintptr_t entry)
{
	size_t index = bmstack_find_and_set(&block_map);
	dassert(index != -1); //< No PID available.

	// Get the address of the entire PCB.
	uintptr_t block = index_to_addr((uintptr_t)blocks, sizeof(block_data_t), index);
	block_data_t* data = (block_data_t*)block;

	// Only map in the 'process' portion.
	process_t* process = &(data->process);
	virtual_alloc(0, (uintptr_t)process, sizeof(process_t));
	memset(process, 0, sizeof(process_t));

	// Determine the version for this process.
	/* Index 0 is reserved (means any version in an ipc port). */
	block_versions[index] = max(1, block_versions[index] + 1);

	process->pid     = index;
	process->version = block_versions[index];

	// Get extra info from the parent, if valid.
	process_t* parent = process_get(ppid);
	process->parent = (parent) ? ppid : 0;
	process->priv   = (parent) ? parent->priv : PRIV_DRIVER;

	process->entry = entry;
	process->addr_space = paging_pas_create();

	// Set up pointers to the other information in the PCB.
	process->messages = data->messages;
	process->threads  = data->threads;

	dtrace("Created process with PID %i.", index);
	return index;
}

//! Free a process control block and its threads.
void process_kill(pid_t pid)
{
	process_t* target = process_get(pid);
	dassert(target);

	// FIXME: Add bitmap function to find first set bit.
	for (size_t i = 0; i != PROCESS_THREAD_MAX; ++i)
	{
		if (bitmap_test(target->thread_info.bitmap, i))
		{
			thread_uid_t uid = { .pid = pid, .tid = i };
			thread_kill(uid);
		}
	}

	// Set the PCB as free and release any allocated objects.
	bmstack_clear(&block_map, pid);
	paging_pas_destroy(target->addr_space);

	// Unmap the entire PCB area.
	/* This will also free the threads for this process. */
	virtual_unmap(0, (uintptr_t)target, sizeof(block_data_t));

	dtrace("Destroyed process with PID %i.", pid);
}

//! Get the actual PCB structure from a PID.
process_t* process_get(pid_t pid)
{
	if (pid >= PROCESS_MAX)
		return NULL;

	// Check for the reserved PIDs.
	if (pid < 2)
		return NULL;

	process_t* proc = (process_t*)index_to_addr((uintptr_t)blocks, sizeof(block_data_t), pid);
	return (bmstack_test(&block_map, pid)) ? proc : NULL;
}
