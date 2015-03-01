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

#include <system/internal.h>
#include <multitask/process.h>
#include <multitask/scheduler.h>
#include <ipc/port.h>
#include <ipc/message.h>
#include <debug/error.h>
#include <horizon/ipc.h>
#include <stdbool.h>
#include <memory.h>

void internal_ksend(ipcport_t port, msgdata_t code, msgdata_t args[MSG_ARGC])
{
	thread_uid_t target_uid = port_to_uid(port, 0);
	dassert(target_uid.raw != 0);

	// FIXME: Just kill the process/thread?
	process_t* target_proc = process_get(target_uid.pid);
	if (target_proc->msg_info.count >= PROCESS_MESSAGE_MAX)
		dpanic("Kernel message could not be sent.");

	// The target may be waiting for the special kernel IPC port.
	thread_t* target = thread_get(target_uid);

	bool woken = false;
	if (target->state == THREAD_STATE_WAITING)
		woken = port_compare(target->syscall_info.wait_for, IPORT_KERNEL);

	struct msg info = {0};
	info.to = port;
	info.code = code;
	memcpy(info.args, args, sizeof(msgdata_t) * MSG_ARGC);

	message_add(target_uid, IPORT_KERNEL, &info, woken);
	if (woken)
		scheduler_add(target_uid);
}
