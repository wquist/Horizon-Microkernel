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
#include <memory.h>

// FIXME: Move the shm PCB info management into its own class.
void syscall_share(struct shm* info, size_t flags)
{
	if (!info)
		return syscall_return_set(-e_badparam);

	thread_t* caller = thread_get(scheduler_curr());
	process_t* owner = process_get(caller->owner);

	// First check if there is already shm being shared.
	if (!((caller->call_data.shm_offer.to) || (flags & SOPT_FORCE)))
		return syscall_return_set(-e_notavail);

	// The target must exist, unless it is any process.
	/* FIXME: Allow IDST_ANY as a valid option? */
	if (!process_get(info->to) && info->to != ICHAN_ANY)
		return syscall_return_set(-e_notavail);

	// The memory must be mapped in the source.
	uintptr_t src = (uintptr_t)(info->addr);
	if (!(info->size))
		return syscall_return_set(-e_badparam);
	if (virtual_is_mapped(owner->pid, src, info->size) != 1)
		return syscall_return_set(-e_badaddr);

	// Make sure the protection is valid.
	switch (info->prot)
	{
		case SPROT_READ:
		case SPROT_WRITE:
			break;
		default:
			return syscall_return_set(-e_badparam);
	}

	// Store the shm info in the TCB of the caller.
	memcpy(&(caller->call_data.shm_offer), info, sizeof(struct shm));

	// The shmid is just a TID for now.
	shmid_t sid = caller->tid;
	syscall_return_set(sid);
}
