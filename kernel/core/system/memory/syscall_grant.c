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
#include <ipc/target.h>
#include <horizon/ipc.h>
#include <horizon/errno.h>

void syscall_grant(struct shm* info, uintptr_t dest)
{
	if (!info)
		return syscall_return_set(-e_badparam);

	thread_t* caller = thread_get(scheduler_curr());
	process_t* owner = process_get(caller->owner);

	// The target has to exist.
	process_t* target = process_get(info->to);
	if (!target)
		return syscall_return_set(-e_notavail);

	// Can only map if higher priv or the parent of the target process.
	if (!((owner->priv > target->priv) || (target->parent == owner->pid)))
		return syscall_return_set(-e_badpriv);

	// The space must exist in the caller, and be free in the target.
	uintptr_t src = (uintptr_t)(info->addr);
	if (!info->size)
		return syscall_return_set(-e_badsize);
	if (virtual_is_mapped(owner->pid, src, info->size) != 1)
		return syscall_return_set(-e_badaddr);
	if (virtual_is_mapped(target->pid, dest, info->size) != 0)
		return syscall_return_set(-e_notavail);

	// Make sure the protection field is valid.
	switch (info->prot)
	{
		case SPROT_READ:
		case SPROT_WRITE:
			// FIXME: Single function so there is not an extra retain-release?
			virtual_share(target->pid, owner->pid, dest, src, info->size, info->prot);
			virtual_unmap(owner->pid, src, info->size);
			break;
		default:
			return syscall_return_set(-e_badparam);
	}

	syscall_return_set(-e_success);
}
