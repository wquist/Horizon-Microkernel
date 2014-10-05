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
#include <ipc/port.h>
#include <util/addr.h>
#include <horizon/ipc.h>
#include <horizon/shm.h>
#include <horizon/errno.h>

void syscall_accept(shmid_t sid, uintptr_t dest, size_t size)
{
	if (!size)
		return syscall_return_set(ESIZE);

	thread_uid_t caller_uid = scheduler_curr();

	// Thread UID == SHM ID for now.
	thread_uid_t target_uid = { .raw = sid };

	thread_t* target = thread_get(target_uid);
	if (!target)
		return syscall_return_set(ENOTAVAIL);

	struct shm* info = &(target->syscall_info.shm_offer);

	// Make sure the caller is allowed to accept.
	if (!ipc_port_compare(info->to, caller_uid))
		return syscall_return_set(EPRIV);

	if (addr_align(dest, ARCH_PGSIZE) != dest)
		return syscall_return_set(EALIGN);
	if (size % ARCH_PGSIZE != 0)
		return syscall_return_set(ESIZE);

	if (virtual_is_mapped(caller_uid.pid, dest, size) != 0)
		return syscall_return_set(EADDR);

	uintptr_t src = (uintptr_t)(info->addr);
	virtual_share(caller_uid.pid, target_uid.pid, dest, src, info->size, info->prot);

	syscall_return_set(info->size);
}
