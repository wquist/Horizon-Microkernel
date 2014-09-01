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

#include "target.h"
#include <multitask/process.h>
#include <horizon/ipc.h>

#define CHAN_VER(x) ((uint16_t)(((x) & 0xFFFF0000) >> 16))
#define CHAN_VER_PID 0
#define CHAN_VER_TID 1

//! Convert an IPC channel format into a TID value.
tid_t ipc_tid_get(ipcchan_t chan)
{
	uint16_t uid = ICHANID(chan);
	if (chan == ICHAN_ANY || chan == ICHAN_KERNEL)
		return uid;

	uint16_t ver = CHAN_VER(chan);
	switch (ver)
	{
		case CHAN_VER_PID: //< The target is only a process.
		{
			process_t* proc = process_get(uid);
			if (proc)
				return proc->threads.slots[0];

			break;
		}
		case CHAN_VER_TID: //< The thread version does not matter.
		{
			return uid;
		}
		default: //< A specific version is targeted.
		{
			thread_t* thread = thread_get(uid);
			if (thread && thread->version == ver)
				return uid;

			break;
		}
	}

	return 0;
}

//! Compare an IPC channel with a thread ID to see if they are equal.
bool ipc_tid_compare(ipcchan_t chan, tid_t caller)
{
	if (chan == ICHAN_ANY)
		return true;
	if (chan == ICHAN_KERNEL)
		return (caller == ICHAN_KERNEL);

	uint16_t uid = ICHANID(chan);
	uint16_t ver = CHAN_VER(chan);

	switch (ver)
	{
		case CHAN_VER_PID:
		{
			thread_t* thread = thread_get(caller);
			if (thread)
				return (uid == thread->owner);

			break;
		}
		case CHAN_VER_TID:
		{
			return (uid == caller);
		}
		default:
		{
			thread_t* thread = thread_get(uid);
			if (thread && thread->version == ver)
				return (uid == caller);

			break;
		}
	}

	return 0;
}

//! Compare an IPC channel with a message structure.
bool ipc_message_compare(ipcchan_t chan, message_t* msg)
{
	if (chan == ICHAN_ANY)
		return true;
	if (chan == ICHAN_KERNEL)
		return (msg->channel == ICHAN_KERNEL);

	uint16_t uid = ICHANID(chan);
	uint16_t ver = CHAN_VER(chan);

	switch (ver)
	{
		case CHAN_VER_PID:
			return (uid == msg->sender);
		case CHAN_VER_TID:
			return (uid == (msg->channel & 0xFFFF));
		default:
			return (chan == msg->channel);
	}
}
