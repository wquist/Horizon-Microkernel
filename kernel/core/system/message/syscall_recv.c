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
#include <util/addr.h>
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

	msgsrc_t from;
	// Peek the message first in case there is a problem with payload.
	uint8_t flags = message_peek(caller->tid, &from);
	thread_t* sender = thread_get(MSRCTID(from));

	if (sender && (flags & MESSAGE_FLAG_PAYLOAD))
	{
		uintptr_t addr_from = sender->call_data.payload_addr;
		uintptr_t addr_to   = (uintptr_t)(dest->payload.buf);
		size_t size_from = sender->call_data.payload_size;
		size_t size_to   = dest->payload.size;

		if (!size_to)
			return syscall_return_set(-e_badsize);
		if (size_to < size_from)
			return syscall_return_set(-e_badsize);
		if (virtual_is_mapped(caller->owner, addr_to, size_from) != 1)
			return syscall_return_set(-e_badaddr);

		process_t* from = process_get(sender->owner);

		// Copy the payload to the current address space.
		uintptr_t end = addr_to + size_from;
		while (addr_to < end)
		{
			uintptr_t aligned = addr_align(addr_from, ARCH_PGSIZE);
			void* phys = paging_mapping_get(from->addr_space, aligned);

			uintptr_t diff = addr_from - aligned;
			void* data = paging_map_temp(phys) + diff;

			size_t to_copy = min(ARCH_PGSIZE - diff, end - addr_to);
			memcpy((void*)addr_to, data, to_copy);

			addr_from += to_copy;
			addr_to   += to_copy;
		}

		scheduler_add(sender->tid);
	}

	// Everything is OK, so actually receive the message.
	message_recv(caller->tid, dest);
	syscall_return_set(-e_success);
}
