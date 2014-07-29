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

void syscall_detach(uint16_t tid, int code)
{
	thread_t* caller = thread_get(scheduler_curr());

	if (tid == TID_SELF)
		tid = caller->tid;

	thread_t* target = thread_get(tid);
	if (!target)
		return syscall_return_set(-e_notavail);

	process_t* owner = process_get(target->owner);
	if (owner->priv > caller->priv)
		return syscall_return_set(-e_badpriv);

	// Kill the thread.
}
