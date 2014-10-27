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
#include <system/internal.h>
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

	service_ids[svc] = port_from_uid(uid);

	if (svc < SVC_IMAX)
		int_callback_set(irq_to_isr(svc), false, irq_callback);
}

//! Get the IPC port of a thread that registered for the given service.
ipcport_t service_get(size_t svc)
{
	dassert(svc < SVCMAX);

	// Port '0' is technically valid, but would be invalid as a service.
	/* The port would be local TID0, but services are always absolute. */
	ipcport_t port = service_ids[svc];
	if (port_to_uid(port, 0).raw != 0)
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

	msgdata_t args[MSG_ARGC] = { 0 };
	internal_ksend(port, irq, args);
}
