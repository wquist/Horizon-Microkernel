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
#include <memory/virtual.h>
#include <multitask/process.h>
#include <multitask/scheduler.h>
#include <util/addr.h>
#include <horizon/errno.h>

void syscall_vmap(uintptr_t dest, size_t size)
{
	if (dest + size >= KERNEL_VIRT_BASE)
		return syscall_return_set(-e_badaddr);
	if (!size)
		return syscall_return_set(-e_badsize);

	thread_t* caller = thread_get(scheduler_curr());
	process_t* owner = process_get(caller->owner);

	uintptr_t actual = addr_align(dest, ARCH_PGSIZE);
	virtual_alloc(owner->pid, actual, size);
	syscall_return_set(actual);
}
