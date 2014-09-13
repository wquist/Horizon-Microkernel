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

#include <system/syscalls.h>
#include <memory/virtual.h>
#include <multitask/process.h>
#include <multitask/scheduler.h>
#include <ipc/port.h>
#include <horizon/ipc.h>
#include <horizon/shm.h>
#include <horizon/errno.h>

void syscall_grant(struct shm* info, uintptr_t dest)
{
	thread_uid_t caller_uid = scheduler_curr();
	if (virtual_is_mapped(caller_uid.pid, (uintptr_t)info, sizeof(struct shm)) != 1)
		return syscall_return_set(EPARAM);

	//
}
