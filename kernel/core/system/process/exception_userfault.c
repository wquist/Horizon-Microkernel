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

#include <system/exceptions.h>
#include <multitask/process.h>
#include <multitask/scheduler.h>

void exception_userfault(isr_t isr)
{
	scheduler_lock();

	thread_uid_t target_uid = scheduler_curr();
	if (target_uid.tid == 0)
	{
		scheduler_purge(target_uid.pid);
		process_kill(target_uid.pid);
	}
	else
	{
		scheduler_remove(target_uid);
		thread_kill(target_uid);
	}

	scheduler_unlock();
}
