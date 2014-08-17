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
#include <ipc/message.h>
#include <multitask/process.h>
#include <multitask/scheduler.h>
#include <horizon/errno.h>

void syscall_drop(struct msg* info)
{
	thread_t* caller = thread_get(scheduler_curr());
	if (caller->messages.count == 0)
		return syscall_return_set(-e_notavail);

	uint8_t flags = message_recv(caller->tid, info);
	thread_t* sender = thread_get(MSRCTID(info->from));

	// If the sender was waiting, it must be woken.
	if (sender && (flags & MESSAGE_FLAG_PAYLOAD))
		scheduler_add(sender->tid);

	syscall_return_set(-e_success);
}
