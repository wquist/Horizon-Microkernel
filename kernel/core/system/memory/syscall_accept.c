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

void syscall_accept(shmid_t sid, uintptr_t dest)
{
	thread_t* caller = thread_get(scheduler_curr());
	process_t* owner = process_get(caller->owner);

	// The shmid is only the TID for now.
	tid_t from_tid = sid & 0xFFFF;
	thread_t* info_from = thread_get(from_tid);
	if (!info_from)
		return syscall_return_set(-e_notavail);

	process_t* map_from = process_get(info_from->owner);

	// Make sure the caller is allowed to accept.
	struct shm* info = &(info_from->call_data.shm_offer);
	if (!ipc_dest_compare(caller->tid, info->to))
		return syscall_return_set(-e_badpriv);

	// The destination space must be unmapped.
	if (virtual_is_mapped(owner->pid, dest, info->size))
		return syscall_return_set(-e_badaddr);

	// The protection was already checked in share, so just map.
	uintptr_t src = (uintptr_t)(info->addr);
	virtual_share(owner->pid, map_from->pid, dest, src, info->size, info->prot);

	// Unset the shm offer so the source knows it was accepted.
	if (info->to != IDST_ANY)
		info->to = 0;

	syscall_return_set(-e_success);
}
