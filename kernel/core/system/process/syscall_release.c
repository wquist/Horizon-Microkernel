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
#include <horizon/proc.h>
#include <horizon/errno.h>

void syscall_release(tid_t tid)
{
	thread_uid_t caller_uid = scheduler_curr();
	if (tid == TID_SELF)
		tid = caller_uid.tid;

	// Cannot release the main thread (use kill instead).
	if (tid == 0)
		return syscall_return_set(EINVALID);

	thread_uid_t target_uid = { .pid = caller_uid.pid, .tid = tid };
	thread_t* target = thread_get(target_uid);
	if (!target)
		return syscall_return_set(ENOTAVAIL);

	// The scheduler must be locked to remove the running thread.
	if (target_uid.tid == caller_uid.tid)
		scheduler_lock();

	thread_kill(caller_uid);

	// Check if the scheduler is locked; the running thread is dead then.
	if (!(scheduler_curr().pid))
		scheduler_unlock();
	else
		syscall_return_set(ENONE);
}
