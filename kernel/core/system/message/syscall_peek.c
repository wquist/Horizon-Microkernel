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
#include <multitask/process.h>
#include <multitask/scheduler.h>
#include <ipc/port.h>
#include <ipc/message.h>
#include <horizon/ipc.h>
#include <horizon/msg.h>
#include <horizon/errno.h>

void syscall_peek()
{
	thread_uid_t caller_uid = scheduler_curr();

	thread_t* caller = thread_get(caller_uid);
	if (!(caller->msg_info.count))
		return syscall_return_set(ENOTAVAIL);

	ipcport_t port;
	bool has_payload = message_peek(caller_uid, &port);
	thread_uid_t sender_uid = port_to_uid(port, 0);

	// The payload can no longer be received if the sender is gone.
	if (!(has_payload && sender_uid.raw != 0))
		return syscall_return_set(0);

	thread_t* sender = thread_get(sender_uid);

	size_t size = sender->syscall_info.payload_size;
	syscall_return_set(size);
}
