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

#define DEST_UID(x)  ((uint16_t)(x & 0xFFFF))
#define DEST_TYPE(x) ((uint16_t)((x >> 16) & 0xFFFF))
#define DEST_TYPE_PID 0x0
#define DEST_TYPE_TID 0x1

//! Convert an ipcdst type into a TID value.
tid_t ipc_dest_get(ipcdst_t dest)
{
	uint16_t uid = DEST_UID(dest);
	if (uid == IDST_ANY || uid == IDST_KERNEL)
		return uid;

	switch (DEST_TYPE(dest))
	{
		case DEST_TYPE_TID:
		{
			return uid;
		}
		case DEST_TYPE_PID:
		{
			process_t* proc = process_get(uid);
			if (proc)
				return proc->threads.slots[0];
		}
	}

	return 0;
}

//! Compare a ipcdst type with a TID to see if they are equal.
bool ipc_dest_compare(ipcdst_t dest, uint16_t caller)
{
	uint16_t uid = DEST_UID(dest);
	if (uid == IDST_ANY)
		return true;

	if (uid == IDST_KERNEL)
		return (caller == IDST_KERNEL);

	switch (DEST_TYPE(dest))
	{
		case DEST_TYPE_TID:
		{
			return (uid == caller);
		}
		case DEST_TYPE_PID:
		{
			thread_t* thread = thread_get(caller);
			if (thread)
				return (uid == thread->owner);
		}
	}

	return false;
}
