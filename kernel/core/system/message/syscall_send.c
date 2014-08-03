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
#include <ipc/message.h>
#include <multitask/process.h>
#include <multitask/scheduler.h>
#include <debug/error.h>
#include <horizon/errno.h>

void syscall_send(struct msg* src)
{
	if (!src)
		return syscall_return_set(-e_badparam);

	thread_t* caller = thread_get(scheduler_curr());
	thread_t* dest   = thread_get(message_dest_get(src->to));

	// The destination must be alive and have room in queue.
	if (!dest)
		return syscall_return_set(-e_notavail);
	if (dest->messages.count >= THREAD_MESSAGE_MAX)
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
		woken = message_dest_compare(dest->call_data.wait_for, caller->tid);

	// Here, 'woken' determines if msg is placed at head or tail of queue.
	message_send(caller->tid, dest->tid, src, woken);
	if (woken)
		scheduler_add(dest->tid);

	// Remember to unlock the scheduler if needed.
	if (!scheduler_curr())
		scheduler_unlock();
	else
		syscall_return_set(-e_success);
}
