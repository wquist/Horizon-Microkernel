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
		uint32_t tid : 6;
		uint32_t pid : 10;
		uint32_t pvn : 8;
		uint32_t tvn : 8;
	};
	uint32_t raw;
};

//! Create a formatted port form the given process/thread identifier.
ipcport_t ipc_port_format(thread_uid_t uid)
{
	process_t* process = process_get(uid.pid);
	dassert(process);

	thread_t* thread = thread_get(uid);
	dassert(thread);

	port_fmt_t port = {0};
	port.tid = thread->tid;
	port.pid = process->pid;
	port.pvn = process->version;
	port.tvn = thread->version;

	return port.raw;
}

//! Get the process and thread ID represented by the given port.
/*! - For special values, PID = 1, TID = value.
 *  - For global values, PID = value, TID = 0.
 *  - For a local TID, PID = 0, TID = value.
 *  - For anything else, both PID and TID are set to actual values.
 *  Only returns true if valid and both process and thread exist.
 */
bool ipc_port_get(ipcport_t port, thread_uid_t* uid)
{
	// First check for global values.
	if (port == IPORT_ANY || port == IPORT_KERNEL)
	{
		uid->pid = 1;
		uid->tid = port;
		return true;
	}

	// If a version is not specified, it must be a generic PID or local TID.
	port_fmt_t fmt = { .raw = port };
	if (!(fmt.pvn) && !(fmt.tvn))
	{
		// This is a local TID.
		if (!(fmt.pid))
		{
			uid->pid = 0;
			uid->tid = fmt.tid;
			return true;
		}

		// It must be a generic PID.
		uid->pid = fmt.pid;
		uid->tid = 0; //< The TID is ignored - always send to main thread.
		return true;
	}

	// Both the process and thread must be alive.
	process_t* process = process_get(fmt.pid);
	if (!process)
		return false;

	thread_uid_t target_uid = { .pid = fmt.pid, .tid = fmt.tid };
	thread_t* thread = thread_get(target_uid);
	if (!thread)
		return false;

	// Versioning exists, so the port may not be valid.
	if (process->version != fmt.pvn || thread->version != fmt.tvn)
		return false;

	uid->pid = fmt.pid;
	uid->tid = fmt.tid;
	return true;
}

//! Test if a port is compatible with the given process and thread.
bool ipc_port_compare(ipcport_t port, thread_uid_t uid)
{
	if (port == IPORT_ANY)
		return true;
	if (port == IPORT_KERNEL)
		return (uid.pid == 1 && uid.tid == IPORT_KERNEL);

	port_fmt_t fmt = { .raw = port };
	if (!(fmt.pvn) && !(fmt.tvn))
	{
		if (!(fmt.pid))
			return (fmt.tid == uid.tid);

		return (fmt.pid == uid.pid);
	}

	process_t* process = process_get(uid.pid);
	if (!process)
		return false;

	thread_t* thread = thread_get(uid);
	if (!thread)
		return false;

	return (process->version == fmt.pvn && thread->version == fmt.tvn);
}
