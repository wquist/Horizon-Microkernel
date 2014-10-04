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

#include "service.h"
#include <arch.h>
#include <multitask/process.h>
#include <multitask/scheduler.h>
#include <ipc/port.h>
#include <ipc/message.h>
#include <debug/error.h>
#include <horizon/svc.h>
#include <horizon/ipc.h>
#include <horizon/msg.h>

static ipcport_t service_ids[SVCMAX] = {0};

static void irq_callback(isr_t, irq_t);

//! Associate a thread UID with a service.
/*! Possibly set up the thread to receive kernel messages. */
void service_register(size_t svc, thread_uid_t uid)
{
	dassert(svc < SVCMAX);
	dassert(!service_get(svc));

	thread_t* thread = thread_get(uid);
	dassert(thread);

	service_ids[svc] = ipc_port_format(uid);

	if (svc < SVC_IMAX)
		int_callback_set(irq_to_isr(svc), false, irq_callback);
}

//! Get the IPC port of a thread that registered for the given service.
ipcport_t service_get(size_t svc)
{
	dassert(svc < SVCMAX);

	// Port '0' is technically valid.
	/* The result of this function will still be 0 in that case. */
	ipcport_t port = service_ids[svc];

	thread_uid_t uid;
	if (ipc_port_get(port, 0, &uid))
		return port;

	if (svc < SVC_IMAX)
		int_callback_set(irq_to_isr(svc), false, NULL);

	return 0;
}

// Send a kernel message to the relevent thread upon an IRQ.
void irq_callback(isr_t isr, irq_t irq)
{
	ipcport_t port = service_get(irq);
	if (!port)
		return;

	// Port should always be valid here.
	thread_uid_t target_uid;
	ipc_port_get(port, 0, &target_uid);

	// FIXME: Just kill the process/thread?
	process_t* target_proc = process_get(target_uid.pid);
	if (target_proc->msg_info.count >= PROCESS_MESSAGE_MAX)
		dpanic("Service could not receive IRQ.");

	// FIXME: Code should be some 'SVC_IRQ_OCCURRED' or similar.
	/* Then data can contain the IRQ number. */
	struct msg info = {0};
	info.to = port;
	info.code = irq;

	// FIXME: Messaging code here is almost duplicate of syscall_send.
	/* Create a new 'internal_msgsend' function for both? */
	thread_t* target = thread_get(target_uid);

	bool woken = false;
	if (target->state == THREAD_STATE_WAITING)
		woken = (target->syscall_info.wait_for == IPORT_KERNEL);

	message_add(target_uid, IPORT_KERNEL, &info, woken);
	if (woken)
		scheduler_add(target_uid);
}
