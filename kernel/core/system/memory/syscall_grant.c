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

void syscall_grant(struct shm* info, uintptr_t dest)
{
	thread_uid_t caller_uid = scheduler_curr();

	// Make sure the parameters are valid.
	if (virtual_is_mapped(caller_uid.pid, (uintptr_t)info, sizeof(struct shm)) != 1)
		return syscall_return_set(EPARAM);
	if (!(info->size))
		return syscall_return_set(ESIZE);

	thread_uid_t target_uid;
	if (!ipc_port_get(info->to, caller_uid.pid, &target_uid))
		return syscall_return_set(EINVALID);

	process_t* target = process_get(target_uid.pid);
	if (!target)
		return syscall_return_set(ENOTAVAIL);

	// Only the parent PID or a higher priv can grant memory.
	process_t* owner = process_get(caller_uid.pid);
	if (!((owner->priv > target->priv) || (target->parent == owner->pid)))
		return syscall_return_set(EPRIV);

	uintptr_t src = (uintptr_t)(info->addr);

	// Make sure the address ranges are aligned.
	if (addr_align(src, ARCH_PGSIZE) != src)
		return syscall_return_set(EALIGN);
	if (addr_align(dest, ARCH_PGSIZE) != dest)
		return syscall_return_set(EALIGN);
	if (info->size % ARCH_PGSIZE != 0)
		return syscall_return_set(ESIZE);

	// The space must exist in the caller, and be free in the target.
	if (virtual_is_mapped(owner->pid, src, info->size) != 1)
		return syscall_return_set(EADDR);
	if (virtual_is_mapped(target->pid, dest, info->size) != 0)
		return syscall_return_set(ENORES);

	// FIXME: Single function so there is not an extra retain/release?
	virtual_share(target->pid, owner->pid, dest, src, info->size, info->prot);
	virtual_unmap(owner->pid, src, info->size);

	syscall_return_set(ENONE);
}
