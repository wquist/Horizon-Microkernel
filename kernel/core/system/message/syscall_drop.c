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

void syscall_drop(struct msg* info)
{
	thread_uid_t caller_uid = scheduler_curr();

	// 'info' can be NULL to be ignored, otherwise it must be valid memory.
	int mapped = virtual_is_mapped(caller_uid.pid, (uintptr_t)info, sizeof(struct msg));
	if (info && mapped != 1)
		return syscall_return_set(EPARAM);

	thread_t* caller = thread_get(caller_uid);
	if (!(caller->msg_info.count))
		return syscall_return_set(ENOTAVAIL);

	// Copy the message info now (contains the sender value needed next).
	bool has_payload = message_remove(caller_uid, info);

	thread_uid_t sender_uid;
	bool valid = ipc_port_get(info->from, caller_uid.pid, &sender_uid);

	// If the sender is alive, it needs to be reawoken after blocking for a payload.
	if (valid && has_payload)
		scheduler_add(sender_uid);

	syscall_return_set(ENONE);
}
