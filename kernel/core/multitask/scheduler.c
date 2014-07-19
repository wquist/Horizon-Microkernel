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

// FIXME: Will have to be modified for multi-core.
static uint16_t active_thread = 0;

static void timer_tick(uint8_t isr, int8_t irq);

//! Initialize the scheduler and start the first usermode process.
/*! scheduler_add must be called prior at least once. */
void scheduler_start()
{
	// A thread must be in queue to start.
	dassert(active_thread);

	thread_t* thread = thread_get(active_thread);
	thread->sched.timeslice = SCHEDULER_TIMESLICE;

	// FIXME: Set the arch timer.
	task_start(&(thread->task));
}

//! Add a thread to the scheduler queue.
void scheduler_add(uint16_t tid)
{
	// The thread must exists and not be in a queue.
	thread_t* target = thread_get(tid);
	dassert(target);
	dassert(target->sched.state != THREAD_STATE_ACTIVE);

	// A thread is active once it is in queue.
	/* This does not mean it is actually running. */
	target->sched.state = THREAD_STATE_ACTIVE;

	// The only thread in queue.
	if (!active_thread)
	{
		// Link it to itself.
		target->sched.queue.next = tid;
		target->sched.queue.prev = tid;
		active_thread = tid;

		return;
	}

	// Thread curr always exists here because of above.
	thread_t* curr = thread_get(active_thread);
	thread_t* prev = thread_get(curr->sched.queue.prev);

	// Put the new thread at the end of its queue.
	target->sched.queue.next = curr->tid;
	target->sched.queue.prev = prev->tid;

	curr->sched.queue.prev = tid;
	prev->sched.queue.next = tid;
}

//! Remove a thread from the schduler queue.
/*! The thread cannot be the the scheduler_curr thread. */
void scheduler_remove(uint16_t tid)
{
	// FIXME: Allow the currently running thread to be removed.

	// The thread must exist in the scheduler queue and not be running.
	thread_t* target = thread_get(tid);
	dassert(target);
	dassert(target->sched.timeslice == 0);
	dassert(target->sched.state == THREAD_STATE_ACTIVE);

	target->sched.state = THREAD_STATE_NEW;

	thread_t* next = thread_get(target->sched.queue.next);
	thread_t* prev = thread_get(target->sched.queue.prev);

	// Remove the target from the linked list.
	next->sched.queue.prev = prev->tid;
	prev->sched.queue.next = next->tid;
}

//! Switch to the next thread in the scheduler queue.
void scheduler_next()
{
	thread_t* curr = thread_get(active_thread);
	curr->sched.timeslice = 0;
	
	active_thread = curr->sched.queue.next;

	thread_t* next = thread_get(active_thread);
	next->sched.timeslice = SCHEDULER_TIMESLICE;

	// Do the threads share the same address space?
	if (curr->owner != next->owner)
		paging_pas_load(process_get(next->owner)->addr_space);

	// Is the current thread the only one in queue?
	if (curr->tid != next->tid)
		task_switch(&(curr->task), &(next->task));
}

//! Get the currently running thread.
uint16_t scheduler_curr()
{
	return active_thread;
}

void timer_tick(uint8_t isr, int8_t irq)
{
	thread_t* curr = thread_get(active_thread);
	if (!curr) //< No thread to switch to.
		return;

	--(curr->sched.timeslice);
	if (!(curr->sched.timeslice))
		scheduler_next();
}
