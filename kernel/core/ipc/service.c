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
#include <multitask/process.h>
#include <horizon/svc.h>
#include <debug/error.h>

static ipcchan_t service_ids[SVCMAX] = {0};

//! Associate a TID with a service number.
/*! Possibly set up the thread to receive kernel message for INTs. */
void service_register(size_t svc, tid_t tid)
{
	dassert(svc < SVCMAX);
	dassert(!service_get(svc));

	thread_t* thread = thread_get(tid);
	dassert(thread);

	service_ids[svc] = ((ipcchan_t)(thread->version) << 16) | tid;

	// FIXME: Register interrupt if needed.
}

//! Get the TID associated with a service number.
/*! Also checks if the TID is still valid, and clears it otherwise. */
ipcchan_t service_get(size_t svc)
{
	dassert(svc < SVCMAX);

	uint16_t tid = service_ids[svc] & 0xFFFF;
	uint16_t ver = service_ids[svc] >> 16;

	// Make sure the service is still valid.
	thread_t* thread = thread_get(tid);
	if (thread && thread->version == ver)
		return service_ids[svc];

	// The service was not valid; reset it.
	service_ids[svc] = 0;
	return 0;
}
