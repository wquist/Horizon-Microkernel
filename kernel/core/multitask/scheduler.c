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

#include "scheduler.h"
#include <arch.h>
#include <multitask/process.h>
#include <debug/error.h>
#include <stdbool.h>

// FIXME: Will have to be modified for multi-core.
static thread_uid_t active_thread = {0};
static bool locked = false;

static void timer_tick(isr_t isr, irq_t irq);

//! Initialize the scheduler and switch to usermode with the first thread.
/*! scheduler_add must be called at least once prior to this function. */
void scheduler_start()
{
	// A thread must be in queue to start.
	dassert(active_thread.raw);

	thread_t* thread = thread_get(active_thread);
	thread->sched_info.timeslice = SCHEDULER_TIMESLICE;

	arch_timer_init(SCHEDULER_FREQ, timer_tick);

	paging_pas_load(process_get(thread->owner)->addr_space);
	task_start(&(thread->task));
}

//! Add a thread to the scheduler queue.
void scheduler_add(thread_uid_t uid)
{
	// The thread must not already be in a scheduler queue.
	thread_t* target = thread_get(uid);
	dassert(target);
	dassert(target->state != THREAD_STATE_ACTIVE);

	target->state = THREAD_STATE_ACTIVE;

	// Is this the only thread in queue?
	if (!(active_thread.pid))
	{
		// Link this thread to itself.
		target->sched_info.next = uid;
		target->sched_info.prev = uid;

		active_thread = uid;
		return;
	}

	// Insert the thread into the linked list.
	/* 'curr' and 'prev' may point to the same thread. */
	thread_t* curr = thread_get(active_thread);
	thread_t* prev = thread_get(curr->sched_info.prev);

	// The new thread is inserted at the end of the queue.
	target->sched_info.next = active_thread;
	target->sched_info.prev = curr->sched_info.prev;

	curr->sched_info.prev = uid;
	prev->sched_info.next = uid;
}

//! Remove a thread from the scheduler queue.
/*! The target thread cannot be the active unless the scheduler is locked. */
void scheduler_remove(thread_uid_t uid)
{
	// The thread must actually be present in a queue.
	thread_t* target = thread_get(uid);
	dassert(target);
	dassert(target->state == THREAD_STATE_ACTIVE);

	// Current thread allowed only if locked.
	dassert(uid.raw != active_thread.raw || locked);

	if (target->sched_info.next.raw == uid.raw)
	{
		// This is the last thread in the queue.
		active_thread.raw = 0;
	}
	else
	{
		// The thread may have been running, so update the active thread.
		if (active_thread.raw == uid.raw)
			active_thread = target->sched_info.next;

		thread_t* next = thread_get(target->sched_info.next);
		thread_t* prev = thread_get(target->sched_info.prev);

		// Remove the target from the linked list.
		next->sched_info.prev = target->sched_info.prev;
		prev->sched_info.next = target->sched_info.next;
	}

	target->state = THREAD_STATE_OLD;
}

//! Remove all threads of a given process from their queues.
void scheduler_purge(pid_t pid)
{
	process_t* process = process_get(pid);
	dassert(process);

	for (size_t i = 0; i != PROCESS_THREAD_MAX; ++i)
	{
		if (bitmap_test(process->thread_info.bitmap, i))
		{
			thread_uid_t uid = { .pid = pid, .tid = i };

			thread_t* target = thread_get(uid);
			if (target->state != THREAD_STATE_ACTIVE)
				continue;

			scheduler_remove(uid);
		}
	}
}

//! Switch to the context of the next thread.
void scheduler_next()
{
	thread_t* curr = thread_get(active_thread);

	// Is the current thread the only one in queue?
	/* No need to do a task switch then. */
	if (active_thread.raw == curr->sched_info.next.raw)
		return;

	active_thread = curr->sched_info.next;
	thread_t* next = thread_get(active_thread);
	dassert(next);

	next->sched_info.timeslice = SCHEDULER_TIMESLICE;

	// Do the threads share the same address space?
	/* Save a directory flush if so. */
	if (curr->owner != next->owner)
		paging_pas_load(process_get(next->owner)->addr_space);

	task_switch(&(curr->task), &(next->task));
}

//! Enable a no-thread state during a context switch.
void scheduler_lock()
{
	locked = true;

	thread_t* curr = thread_get(active_thread);
	task_preserve(&(curr->task));
}

//! Re-enable the scheduler before returning from a context switch.
void scheduler_unlock()
{
	// FIXME: Wait for an interrupt instead of panicking.
	dassert(active_thread.raw);

	locked = false;

	// FIXME: Store last addr space to avoid reloading PAS every time?
	thread_t* curr = thread_get(active_thread);
	paging_pas_load(process_get(curr->owner)->addr_space);
	task_switch(NULL, &(curr->task));
}

//! Get the currently running thread.
/*! Returns zero regardless if the scheduler is locked. */
thread_uid_t scheduler_curr()
{
	return (locked) ? (thread_uid_t){0} : active_thread;
}

// Update the timeslice of the running thread and possibly switch.
void timer_tick(isr_t isr, irq_t irq)
{
	// A thread should always exist at this point.
	thread_t* curr = thread_get(active_thread);
	dassert(curr);

	curr->sched_info.timeslice -= 1;
	if (!(curr->sched_info.timeslice))
		scheduler_next();
}
