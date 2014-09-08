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
#include <memory/virtual.h>
#include <util/compare.h>
#include <util/addr.h>
#include <util/bitmap.h>
#include <debug/log.h>
#include <debug/error.h>

//! Create a new thread in the given process.
tid_t thread_new(pid_t pid, uintptr_t entry)
{
	process_t* owner = process_get(pid);
	dassert(owner);

	size_t index = bitmap_find_and_set(owner->thread_info.bitmap, PROCESS_THREAD_MAX);
	dassert(index != -1); // A free thread slot must exist.

	thread_t* thread = &(owner->threads[index]);
	// This specific thread may not be mapped in yet.
	virtual_alloc(0, (uintptr_t)thread, sizeof(thread_t));
	memset(thread, 0, sizeof(thread_t));

	// Update the version number for this thread (0 is reserved).
	owner->thread_info.versions[index] = max(1, owner->thread_info.versions[index] + 1);
	owner->thread_info.count += 1;

	thread->tid     = index;
	thread->version = owner->thread_info.versions[index];
	thread->owner   = pid;

	task_init(&(thread->task));
	thread->task.entry = (entry) ? entry : owner->entry;

	dtrace("Created thread with TID %i, owned by PID %i.", index, pid);
	return index;
}

//! Destroy a thread in a given process.
/*! Does not free the thread memory; all PCB memory is freed with process_kill. */
void  thread_kill(pid_t pid, tid_t tid)
{
	process_t* owner = process_get(pid);
	dassert(owner);
	dassert(bitmap_test(owner->thread_info.bitmap, tid));

	bitmap_clear(owner->thread_info.bitmap, tid);
	owner->thread_info.count -= 1;

	dtrace("Destroyed thread with TID %i in PID %i.", tid, pid);
}

//! Get the specificed thread for a given process.
thread_t* thread_get(pid_t pid, tid_t tid)
{
	if (tid >= PROCESS_THREAD_MAX)
		return NULL;

	process_t* owner = process_get(pid);
	dassert(owner);

	thread_t* thread = &(owner->threads[tid]);
	return (bitmap_test(owner->thread_info.bitmap, tid)) ? thread : NULL;
}
