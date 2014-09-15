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
#include <ipc/message.h>
#include <util/addr.h>
#include <util/compare.h>
#include <horizon/ipc.h>
#include <horizon/msg.h>
#include <horizon/errno.h>
#include <memory.h>

void syscall_recv(struct msg* dest)
{
	thread_uid_t caller_uid = scheduler_curr();

	if (virtual_is_mapped(caller_uid.pid, (uintptr_t)dest, sizeof(struct msg) != 1))
		return syscall_return_set(EPARAM);

	thread_t* caller = thread_get(caller_uid);
	if (!(caller->msg_info.count))
		return syscall_return_set(ENOTAVAIL);

	// Peek the message first in case there are any issues.
	/* The dest payload info has not been confirmed valid yet. */
	ipcport_t port;
	bool has_payload = message_peek(caller_uid, &port);

	thread_uid_t sender_uid;
	bool valid = ipc_port_get(port, caller_uid.pid, &sender_uid);

	size_t recv_size = 0;
	if (has_payload && valid)
	{
		thread_t* sender = thread_get(sender_uid);

		uintptr_t addr_from = sender->syscall_info.payload_addr;
		uintptr_t addr_to   = (uintptr_t)(dest->payload.buf);
		size_t size_from = sender->syscall_info.payload_size;
		size_t size_to   = dest->payload.size;

		if (!size_to || size_to < size_from)
			return syscall_return_set(ESIZE);
		if (virtual_is_mapped(caller_uid.pid, addr_to, size_from) != 1)
			return syscall_return_set(EADDR);

		process_t* sender_proc = process_get(sender->owner);

		// Copy the payload into the current address space.
		uintptr_t end = addr_to + size_from;
		while (addr_to < end)
		{
			uintptr_t addr_aligned = addr_align(addr_from, ARCH_PGSIZE);
			void* phys = paging_mapping_get(sender_proc->addr_space, addr_aligned);

			uintptr_t diff = addr_from - addr_aligned;
			void* data = paging_map_temp(phys) + diff;

			size_t to_copy = min(ARCH_PGSIZE - diff, end - addr_to);
			memcpy((void*)addr_to, data, to_copy);

			addr_from += to_copy;
			addr_to   += to_copy;
		}

		recv_size = size_from;
		scheduler_add(sender_uid);
	}

	// The payload does not exist or was valid - get the rest.
	message_remove(caller_uid, dest);
	syscall_return_set(recv_size);
}
