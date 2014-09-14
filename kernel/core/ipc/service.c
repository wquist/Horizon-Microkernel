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
#include <ipc/port.h>
#include <debug/error.h>
#include <horizon/svc.h>

static ipcport_t service_ids[SVCMAX] = {0};

//! Associate a thread UID with a service.
/*! Possibly set up the thread to receive kernel messages. */
void service_register(size_t svc, thread_uid_t uid)
{
	dassert(svc < SVCMAX);
	dassert(!service_get(svc));

	thread_t* thread = thread_get(uid);
	dassert(thread);

	service_ids[svc] = ipc_port_format(uid);

	// Set up a callback.
}

ipcport_t service_get(size_t svc)
{
	dassert(svc < SVCMAX);

	// Port '0' is technically valid.
	/* The result of this function will still be 0 in that case. */
	ipcport_t port = service_ids[svc];

	thread_uid_t uid;
	if (ipc_port_get(port, &uid))
		return port;

	// Remove the callback

	return 0;
}
