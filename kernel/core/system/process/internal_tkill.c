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

#include <system/internal.h>
#include <multitask/process.h>
#include <multitask/scheduler.h>

void internal_tkill(thread_uid_t uid)
{
	thread_uid_t caller_uid = scheduler_curr();
	if (caller_uid.pid == uid.pid)
		scheduler_lock();

	if (uid.tid == 0)
	{
		scheduler_purge(uid.pid);
		process_kill(uid.pid);
	}
	else
	{
		scheduler_remove(uid);
		thread_kill(uid);
	}

	if (!(scheduler_curr().pid))
		scheduler_unlock();
}
