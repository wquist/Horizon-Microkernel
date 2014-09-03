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
#include <ipc/target.h>
#include <ipc/message.h>
#include <horizon/svc.h>
#include <horizon/ipc.h>
#include <debug/error.h>

static ipcchan_t service_ids[SVCMAX] = {0};

static void service_int(isr_t isr, irq_t irq);

//! Associate a TID with a service number.
/*! Possibly set up the thread to receive kernel message for INTs. */
void service_register(size_t svc, tid_t tid)
{
	dassert(svc < SVCMAX);
	dassert(!service_get(svc));

	thread_t* thread = thread_get(tid);
	dassert(thread);

	service_ids[svc] = ((ipcchan_t)(thread->version) << 16) | tid;

	if (svc < SVC_IMAX)
		int_callback_set(irq_to_isr(svc), false, service_int);
}

//! Get the TID associated with a service number.
/*! Also checks if the TID is still valid, and clears it otherwise. */
ipcchan_t service_get(size_t svc)
{
	dassert(svc < SVCMAX);

	// The service may not be set at all.
	ipcchan_t sid = service_ids[svc];
	uint16_t tid = sid & 0xFFFF;
	uint16_t ver = sid >> 16;

	// Make sure the service is still valid.
	thread_t* thread = thread_get(tid);
	if (thread && thread->version == ver)
		return sid;

	// The service was not valid; reset it.
	service_ids[svc] = 0;
	return 0;
}

void service_int(isr_t isr, irq_t irq)
{
	ipcchan_t target = service_get(irq);
	if (!target)
	{
		int_callback_set(irq, false, NULL);
		return;
	}

	struct msg int_msg = {0};
	int_msg.to = target;

	// FIXME: Make some libh constants for here.
	int_msg.code = 0;
	int_msg.arg  = irq;

	thread_t* dest = thread_get(ipc_tid_get(target));

	bool woken = false;
	if (dest->sched.state == THREAD_STATE_WAITING)
		woken = ipc_tid_compare(dest->call_data.wait_for, ICHAN_KERNEL);

	message_send(ICHAN_KERNEL, dest->tid, &int_msg, woken);
	if (woken)
		scheduler_add(dest->tid);
}
