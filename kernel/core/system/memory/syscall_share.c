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
void syscall_share(struct shm* info)
{
	if (!info)
		return syscall_return_set(-e_badparam);

	thread_t* caller = thread_get(scheduler_curr());
	process_t* owner = process_get(caller->owner);

	// FIXME: The target always has to exist?
	if (!process_get(info->to) && info->to != IDST_ANY)
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

	// Store the shm info in the PCB of the caller.
	uint8_t slot = (owner->call_data.shm_next)++;
	memcpy(&(owner->call_data.shm_slots[slot]), info, sizeof(struct shm));

	// The slots are a circular buffer, check if it needs to loop.
	/* FIXME: Get rid of the magic number. */
	if (owner->call_data.shm_next >= 16)
		owner->call_data.shm_next = 0;

	// The shmid is formatted as: |avail (8)|PID (16)|index (8)|
	shmid_t sid = (shmid_t)((shmid_t)(owner->pid) << 8) | ((shmid_t)(slot));
	syscall_return_set(sid);
}
