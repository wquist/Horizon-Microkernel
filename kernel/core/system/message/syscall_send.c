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
#include <horizon/ipc.h>
#include <horizon/msg.h>
#include <horizon/errno.h>

void syscall_send(struct msg* src)
{
	thread_uid_t caller_uid = scheduler_curr();

	if (virtual_is_mapped(caller_uid.pid, (uintptr_t)src, sizeof(struct msg)) != 1)
		return syscall_return_set(EPARAM);
	if (ipc_port_compare(src->to, caller_uid))
		return syscall_return_set(EPARAM);

	thread_uid_t target_uid;
	if (!ipc_port_get(src->to, caller_uid.pid, &target_uid))
		return syscall_return_set(EINVALID);

	thread_t* target = thread_get(target_uid);
	if (!target)
		return syscall_return_set(ENOTAVAIL);

	process_t* target_proc = process_get(target_uid.pid);
	if (target_proc->msg_info.count >= PROCESS_MESSAGE_MAX)
		return syscall_return_set(ENORES);

	// Set the return value now.
	/* With a payload, the caller may be removed from queue. */
	syscall_return_set(ENONE);

	// A payload requires the sender to block.
	if (src->payload.buf)
	{
		uintptr_t addr = (uintptr_t)(src->payload.buf);
		size_t size = src->payload.size;

		if (!size)
			return syscall_return_set(ESIZE);
		if (virtual_is_mapped(caller_uid.pid, addr, size) != 1)
			return syscall_return_set(EADDR);

		// The running thread will be removed.
		scheduler_lock();
		scheduler_remove(caller_uid);

		thread_t* caller = thread_get(caller_uid);
		caller->state = THREAD_STATE_SENDING;

		caller->syscall_info.payload_addr = addr;
		caller->syscall_info.payload_size = size;
	}

	// Try to wake up the receiver if necessary.
	bool woken = false;
	if (target->state == THREAD_STATE_WAITING)
		woken = ipc_port_compare(target->syscall_info.wait_for, target_uid);

	ipcport_t port = ipc_port_format(caller_uid);

	// Here, 'woken' determines if msg is placed at the head (next in queue) or tail.
	message_add(target_uid, port, src, woken);
	if (woken)
		scheduler_add(target_uid);

	// Remember to unlock the scheduler if needed (return code was set above).
	if (!(scheduler_curr().pid))
		scheduler_unlock();
}
