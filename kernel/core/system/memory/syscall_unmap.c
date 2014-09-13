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
#include <horizon/priv.h>
#include <horizon/errno.h>

void syscall_unmap(uintptr_t addr, size_t size)
{
	if (addr + size > KERNEL_VIRT_BASE)
		return syscall_return_set(EADDR);
	if (!size)
		return syscall_return_set(ESIZE);

	if (addr_align(addr, ARCH_PGSIZE) != addr)
		return syscall_return_set(EALIGN);
	if (size % ARCH_PGSIZE != 0)
		return syscall_return_set(ESIZE);

	// Only processes that can map can unmap.
	process_t* owner = process_get(scheduler_curr().pid);
	if (owner->priv < PRIV_SERVER)
		return syscall_return_set(EPRIV);

	// The entire target region must be mapped in.
	/* Works with a mix of vmap and pmap memory. */
	if (virtual_is_mapped(owner->pid, addr, size) != 1)
		return syscall_return_set(ENOTAVAIL);

	virtual_unmap(owner->pid, addr, size);
	syscall_return_set(ENONE);
}
