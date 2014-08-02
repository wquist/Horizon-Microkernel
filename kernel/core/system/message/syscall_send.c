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

#include <system/syscalls.h>
#include <arch.h>
#include <memory/virtual.h>
#include <multitask/process.h>
#include <multitask/scheduler.h>
#include <debug/error.h>
#include <horizon/errno.h>

// FIXME: Add some message utilities to get destination thread, etc.
void syscall_send(struct msg* src)
{
	if (!src)
		return syscall_return_set(-e_badparam);

	thread_t* caller = thread_get(scheduler_curr());

	thread_t* dest;
	switch ((src->to >> 16) & 0xFFFF) //< Get the destination type.
	{
		case MTOTID: //< Send to a specific TID.
		{
			dest = thread_get(src->to & 0xFFFF);
			break;
		}
		case MTOPID: //< Send to the main thread of a PID.
		{
			process_t* owner = process_get(src->to & 0xFFFF);
			if (!owner)
				return syscall_return_set(-e_notavail);

			dest = thread_get(owner->threads.slots[0]);
			break;
		}
		default: return syscall_return_set(-e_badparam);
	}

	// The destination must be alive.
	if (!dest)
		return syscall_return_set(-e_notavail);

	// Extra information means the sender blocks.
	if (src->payload.buf)
	{
		uintptr_t addr = (uintptr_t)(src->payload.buf);
		size_t size = src->payload.size;

		// The entire space must be mapped.
		if (!size)
			return syscall_return_set(-e_badsize);
		if (virtual_is_mapped(caller->owner, addr, size) != 1)
			return syscall_return_set(-e_badaddr);

		// The running thread will be removed from queue.
		scheduler_lock();

		caller->call_data.payload_addr = addr;
		caller->call_data.payload_size = size;

		// Put the sender into a waiting state.
		scheduler_remove(caller->tid);
		caller->sched.state = THREAD_STATE_SENDING;
	}

	// Try to wake up the thread if needed.
	bool woken = false;
	if (dest->sched.state == THREAD_STATE_WAITING)
	{
		uint16_t target_id = 0;
		switch ((dest->call_data.waiting_for >> 16) & 0xFFFF)
		{
			case MTOTID:
				target_id = caller->tid;
				break;
			case MTOPID:
				target_id = caller->owner;
				break;
			case 0xFFFF:
				target_id = 0xFFFF;
				break;
			default: dpanic("Invalid wait type for receiving thread.");
		}

		uint16_t twait = dest->call_data.waiting_for & 0xFFFF;
		if (twait == target_id || target_id == 0xFFFF)
			woken = true;
	}

	// Here, 'woken' determines if msg is placed at head or tail of queue.
	message_send(caller->tid, dest->tid, src, woken);
	if (woken)
		scheduler_add(dest->tid);

	// Remember to unlock the scheduler if needed.
	if (!scheduler_curr())
		scheduler_unlock();
}
