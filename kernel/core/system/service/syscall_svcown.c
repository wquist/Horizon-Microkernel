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
#include <arch.h>
#include <multitask/process.h>
#include <multitask/scheduler.h>
#include <ipc/service.h>
#include <horizon/priv.h>
#include <horizon/svc.h>
#include <horizon/errno.h>

void syscall_svcown(svc_t svc)
{
	if (svc >= SVCMAX)
		return syscall_return_set(EPARAM);
	if (service_get(svc) != 0)
		return syscall_return_set(ENOTAVAIL);

	thread_uid_t caller_uid = scheduler_curr();

	// Only servers and drivers can register as services.
	process_t* owner = process_get(caller_uid.pid);
	if (owner->priv < PRIV_SERVER)
		return syscall_return_set(EPRIV);

	service_register(svc, caller_uid);
	syscall_return_set(ENONE);
}
