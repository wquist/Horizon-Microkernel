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
#include <horizon/errno.h>

void syscall_dispatch(uintptr_t entry, uintptr_t stack)
{
	pid_t owner = scheduler_curr().pid;

	// Create a new thread under the caller process.
	thread_uid_t uid = thread_new(owner, entry);
	thread_get(uid)->task.stack = stack;

	scheduler_add(uid);
	syscall_return_set(uid.tid);
}
