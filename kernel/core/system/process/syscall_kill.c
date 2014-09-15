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

void syscall_kill(pid_t pid)
{
	process_t* owner = process_get(scheduler_curr().pid);
	if (pid == PID_SELF)
		pid = owner->pid;

	process_t* target = process_get(pid);
	if (!target)
		return syscall_return_set(ENOTAVAIL);

	// The caller must have an equal or greater priv than the target.
	if (owner->priv < target->priv)
		return syscall_return_set(EPRIV);

	// The currently running thread may be contained in this process.
	if (target->pid == owner->pid)
		scheduler_lock();

	scheduler_purge(target->pid);
	process_kill(target->pid);

	if (!(scheduler_curr().pid))
		scheduler_unlock();
	else
		syscall_return_set(ENONE);
}
