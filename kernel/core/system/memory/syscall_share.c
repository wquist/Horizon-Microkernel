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
#include <memory.h>

void syscall_share(struct shm* info)
{
	thread_uid_t caller_uid = scheduler_curr();

	if (virtual_is_mapped(caller_uid.pid, (uintptr_t)info, sizeof(struct shm)) != 1)
		return syscall_return_set(EPARAM);
	if (!(info->size))
		return syscall_return_set(ESIZE);

	thread_uid_t target_uid;
	if (!ipc_port_get(info->to, caller_uid.pid, &target_uid))
		return syscall_return_set(EINVALID);

	// If a specific dest is given, the process must exist.
	process_t* target = process_get(target_uid.pid);
	if (!(target || (target_uid.pid == 1 && target_uid.tid == IPORT_ANY)))
		return syscall_return_set(ENOTAVAIL);

	uintptr_t src = (uintptr_t)(info->addr);

	// The memory must exist in the source.
	if (addr_align(src, ARCH_PGSIZE) != src)
		return syscall_return_set(EALIGN);
	if (src % ARCH_PGSIZE != 0)
		return syscall_return_set(ESIZE);

	// Store the SHM in the TCB of the caller.
	thread_t* caller = thread_get(caller_uid);
	memcpy(&(caller->syscall_info.shm_offer), info, sizeof(struct shm));

	// The SHMID is equivalent to a thread UID?
	shmid_t sid = caller_uid.raw;
	syscall_return_set(sid);
}
