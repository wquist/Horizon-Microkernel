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
#include <ipc/message.h>
#include <multitask/process.h>
#include <multitask/scheduler.h>
#include <util/compare.h>
#include <horizon/errno.h>
#include <memory.h>

void syscall_recv(struct msg* dest)
{
	if (!dest)
		return syscall_return_set(-e_badparam);

	thread_t* caller = thread_get(scheduler_curr());
	if (caller->messages.count == 0)
		return syscall_return_set(-e_notavail);

	uint8_t flags = message_recv(caller->tid, dest);
	thread_t* sender = thread_get(MSRCTID(dest->from));

	if (sender && (flags & MESSAGE_FLAG_PAYLOAD))
	{
		uintptr_t addr = sender->call_data.payload_addr;
		uintptr_t size = sender->call_data.payload_size;

		// FIXME: Only peek message if something is wrong.
		/* Then the caller can try again with more size. */
		if (!size)
			return syscall_return_set(-e_badsize);
		if (virtual_is_mapped(caller->owner, addr, size) != 1)
			return syscall_return_set(-e_badaddr);

		process_t* from = process_get(sender->owner);

		// Copy the payload to the current address space.
		uintptr_t end = addr + size;
		while (addr < end)
		{
			void* phys = paging_mapping_get(from->addr_space, addr);
			void* data = paging_map_temp(phys);
			memcpy((void*)addr, data, min(ARCH_PGSIZE, end - addr));

			addr += ARCH_PGSIZE;
		}

		scheduler_add(sender->tid);
	}

	syscall_return_set(-e_success);
}
