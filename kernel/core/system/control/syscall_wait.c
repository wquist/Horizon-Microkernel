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
#include <multitask/process.h>
#include <multitask/scheduler.h>
#include <ipc/target.h>
#include <ipc/message.h>
#include <horizon/ipc.h>
#include <horizon/errno.h>

void syscall_wait(ipcdst_t sender)
{
	// The desired message may already by in queue.
	thread_t* caller = thread_get(scheduler_curr());
	if (message_find(caller->tid, sender))
		return syscall_return_set(-e_success); //< Now use 'recv' to get it.

	// The dest thread must exist.
	tid_t wait_for = ipc_dest_get(sender);
	thread_t* target = thread_get(wait_for);
	if (!target)
	{
		// The target may be any thread or the kernel.
		if (wait_for != IDST_ANY && wait_for != IDST_KERNEL)
			return syscall_return_set(-e_notavail);
	}

	// The caller is going to be removed.
	scheduler_lock();
	scheduler_remove(caller->tid);
	// Put the thread into the generic waiting state.
	caller->sched.state = THREAD_STATE_WAITING;

	// FIXME: Do not set the success code early.
	syscall_return_set(-e_success);
	caller->call_data.wait_for = wait_for;

	// Do not return since the thread is gone now.
	scheduler_unlock();
}
