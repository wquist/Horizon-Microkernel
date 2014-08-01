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

// FIXME: Pretty much exactly the same as process allocating, etc.

#include "process.h"
#include <memory/region.h>
#include <memory/virtual.h>
#include <multitask/scheduler.h>
#include <util/bmstack.h>
#include <util/addr.h>
#include <debug/log.h>
#include <debug/error.h>
#include <memory.h>

static thread_t* blocks = NULL;
static bmstack_t block_map = {0};

//! Initialize the thread bitmap.
void thread_init()
{
	dassert(sizeof(thread_t) <= THREAD_BLOCK_SIZE);

	// Reserve space for the TCBs.
	blocks = (thread_t*)region_reserve(THREAD_MAX * THREAD_BLOCK_SIZE);

	// Map in memory for the bitmap.
	uintptr_t map_start = region_reserve(BMSTACK_SIZE(THREAD_MAX));
	bmstack_init(&block_map, (void*)map_start);
	virtual_alloc(0, map_start, BMSTACK_SIZE(THREAD_MAX));

	// Only TID 0 is reserved.
	bmstack_set(&block_map, 0);

	bmstack_link(&block_map, THREAD_MAX);
	dtrace("Initialized thread bitmap. (%iKB)", BMSTACK_SIZE(THREAD_MAX)/1024);
}

//! Allocate a new thread for the given process.
/*! If entry is passed as 0, the process's entry point is used. */
tid_t thread_new(pid_t pid, uintptr_t entry)
{
	process_t* owner = process_get(pid);
	dassert(owner); //< The process must be alive.
	dassert(owner->threads.count < PROCESS_THREAD_MAX); // Is there space?

	size_t index = bmstack_find_and_set(&block_map);
	dassert(index != -1); //< No more TCBs available.

	uintptr_t block = index_to_addr((uintptr_t)blocks, THREAD_BLOCK_SIZE, index);
	virtual_alloc(0, block, THREAD_BLOCK_SIZE); //< Make sure the TCB is mapped.

	thread_t* thread = (thread_t*)block;
	memset(thread, 0, THREAD_BLOCK_SIZE);

	thread->tid   = index;
	thread->owner = pid;

	task_init(&(thread->task));
	thread->task.entry = (entry) ? entry : owner->entry;

	// Add the thread to a process thread slot and update the bitmap.
	/* FIXME: Should the thread add itself or the process add the thread? */
	size_t slot = bitmap_find_and_set(owner->threads.bitmap, PROCESS_THREAD_MAX);
	owner->threads.slots[slot] = index;
	++(owner->threads.count);

	thread->lid = slot;
	thread->sched.state = THREAD_STATE_NEW; //< A thread is new until it is scheduled.

	dtrace("Created thread with TID %i, owned by PID %i.", index, pid);
	return (tid_t)index;
}

//! Destroy a thread and remove it from the owner process.
void thread_kill(tid_t tid)
{
	thread_t* target = thread_get(tid);
	dassert(target);

	if (target->sched.state == THREAD_STATE_ACTIVE)
		scheduler_remove(tid);

	// FIXME: Destory any IPC data (messages, etc)?

	process_t* owner = process_get(target->owner);
	bitmap_clear(owner->threads.bitmap, target->lid);
	--(owner->threads.count);

	bmstack_clear(&block_map, tid);
	dtrace("Destroyed thread with TID %i, owned by PID %i.", tid, owner->pid);
}

//! Get the actual TCB structure for a given thread.
thread_t* thread_get(tid_t tid)
{
	dassert(tid < THREAD_MAX);

	// Check for reserved TIDs.
	if (tid == 0)
		return NULL;

	if (!bmstack_test(&block_map, tid))
		return NULL;
	else
		return (thread_t*)index_to_addr((uintptr_t)blocks, THREAD_BLOCK_SIZE, tid);
}
