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

#include <system/exceptions.h>
#include <multitask/process.h>
#include <multitask/scheduler.h>
#include <ipc/service.h>
#include <system/internal.h>
#include <horizon/svc.h>
#include <horizon/msg.h>

void exception_userfault(isr_t isr)
{
	thread_uid_t target_uid = scheduler_curr();
	internal_tkill(target_uid);

	// Only a thread was killed, not the entire process.
	if (target_uid.tid != 0)
		return;

	// Send a message to the process manager telling which PID died.
	ipcport_t svc_port = service_get(SVC_PROCMGR);
	if (svc_port)
	{
		// FIXME: Add exception code. (SEGFAULT, etc.)
		msgdata_t data[MSG_ARGC] = { target_uid.pid };
		internal_ksend(svc_port, SVC_PROCMGR, data);
	}
}
