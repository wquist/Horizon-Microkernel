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
#include <ipc/message.h>
#include <ipc/port.h>
#include <horizon/errno.h>

void syscall_wait(ipcport_t sender)
{
	thread_uid_t caller_uid = scheduler_curr();

	// The desired message may already be in queue.
	if (message_find(caller_uid, sender))
		return syscall_return_set(ENONE); //< Use 'revc' to get it.

	// Find what thread the caller is waiting for.
	thread_uid_t target_uid;
	if (!ipc_port_get(sender, &target_uid))
		return syscall_return_set(EINVALID);

	// Make sure whatever is being waited on actually exists.
	if (target_uid.pid == 0)
	{
		// This is referencing a local thread.
		target_uid.pid = caller_uid.pid;

		thread_t* target = thread_get(target_uid);
		if (!target)
			return syscall_return_set(ENOTAVAIL);
	}
	else if (target_uid.pid > 1)
	{
		// This is referencing a global process or specific thread.
		thread_t* target = thread_get(target_uid);
		if (!target)
			return syscall_return_set(ENOTAVAIL);
	}

	// Set the return code now; not available once thread is removed.
	syscall_return_set(ENONE);

	scheduler_lock();
	scheduler_remove(caller_uid);

	thread_t* caller = thread_get(caller_uid);
	caller->state = THREAD_STATE_WAITING;
	caller->syscall_info.wait_for = target_uid;

	// Will automatically switch to the next thread in queue.
	scheduler_unlock();
}
