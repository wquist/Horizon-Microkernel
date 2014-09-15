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
#include <horizon/priv.h>
#include <horizon/errno.h>

void syscall_launch(pid_t pid, uintptr_t entry)
{
	process_t* owner = process_get(scheduler_curr().pid);

	process_t* target = process_get(pid);
	if (!target)
		return syscall_return_set(ENOTAVAIL);

	// The target process must not have any threads running.
	if (target->thread_info.count > 0)
		return syscall_return_set(EINVALID);
	// The process can only be launched by its parent or a server/driver.
	if (!(target->pid == owner->pid || owner->priv > PRIV_NONE))
		return syscall_return_set(EPRIV);

	// The main thread will start at 'entry'.
	target->entry = entry;
	thread_uid_t uid = thread_new(pid, 0);

	scheduler_add(uid);
	syscall_return_set(ENONE);
}
