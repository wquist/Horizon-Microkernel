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

#include "port.h"
#include <multitask/process.h>
#include <debug/error.h>
#include <horizon/ipc.h>

typedef union port_fmt port_fmt_t;
union port_fmt
{
	struct __packed
	{
		uint32_t pid : 10;
		uint32_t pvn : 8;
		uint32_t tid : 6;
		uint32_t tvn : 8;
	};
	uint32_t raw;
};

//! Create a formatted port form the given process/thread identifier.
ipcport_t port_from_uid(thread_uid_t uid)
{
	process_t* process = process_get(uid.pid);
	dassert(process);

	thread_t* thread = thread_get(uid);
	dassert(thread);

	port_fmt_t port = {0};
	port.pid = process->pid;
	port.pvn = process->version;
	port.tid = thread->tid;
	port.tvn = thread->version;

	return port.raw;
}

//! Get the process and thread ID represented by the given port.
/*! - For special values, PID = 1, TID = value.
 *  - For global values, PID = value, TID = 0.
 *  - For a local TID, PID = 0, TID = value.
 *  - For anything else, both PID and TID are set to actual values.
 *  Returns PID0 and TID0 if invalid/unable to convert.
 *  FIXME: returns 0 as invalid, although 0 technically represents local TID0.
 *  - Should not actually cause any problems, though.
 */
thread_uid_t port_to_uid(ipcport_t port, pid_t parent)
{
	port_fmt_t fmt = { .raw = port };

	// First check for global values.
	if (port == IPORT_ANY || port == IPORT_KERNEL)
		return (thread_uid_t){ .pid = 1, .tid = fmt.tid };

	// If a version is not specified, it must be a generic PID or local TID.
	if (!(fmt.pvn) && !(fmt.tvn))
	{
		// This is a local TID.
		if (!(fmt.pid))
			return (thread_uid_t){ .pid = parent, .tid = fmt.tid };

		// It must be a generic PID.
		/* The TID is always set the main thread in this case. */
		return (thread_uid_t){ .pid = fmt.pid, .tid = 0 };
	}

	// If versioning is specified, the process and thread must be alive.
	thread_uid_t target_uid = { .pid = fmt.pid, .tid = fmt.tid };

	// thread_get returns NULL if the process does not exist as well.
	thread_t* thread = thread_get(target_uid);
	if (!thread)
		return (thread_uid_t){0};

	// The port points to a valid thread, but versioning may be invalid.
	/* The thread exists, so the process must exist as well. */
	process_t* process = process_get(target_uid.pid);
	if (process->version != fmt.pvn || thread->version != fmt.tvn)
		return (thread_uid_t){0};

	// The port is valid.
	return (thread_uid_t){ .pid = fmt.pid, .tid = fmt.tid };
}

//! Test if two ports represent equivalent destinations.
bool port_compare(ipcport_t port1, ipcport_t port2)
{
	// Test for the wildcard ports first.
	if (port1 == IPORT_ANY || port2 == IPORT_ANY)
		return true;

	port_fmt_t fmt1 = { .raw = port1 };
	port_fmt_t fmt2 = { .raw = port2 };

	// If one port lacks version info, only test the PID/TID.
	if ((!(fmt1.pvn) && !(fmt1.tvn)) || (!(fmt2.pvn) && !(fmt2.tvn)))
		return ((fmt1.pid == fmt2.pid) && (fmt1.tid == fmt2.tid));

	// Both ports are versioned, so they are equivalent if equal.
	return (port1 == port2);
}
