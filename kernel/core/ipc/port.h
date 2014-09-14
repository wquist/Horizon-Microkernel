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

/*! \file core/ipc/port.h
 *  \date September 2014
 */

#pragma once

#include <multitask/thread.h>
#include <horizon/types.h>
#include <stdbool.h>

//! The IPC port data type can represent multiple target destinations:
/*! Global values:     ANY (0) or KERNEL (1).
 *  A local thread:    the TID.
 *  A specific PID:    (the PID) << 6
 *  An exact receiver: TVN << 8 | PVN << 8 | (the PID) << 6 | (the TID)
 */

ipcport_t ipc_port_format(thread_uid_t uid);

bool ipc_port_get(ipcport_t port, thread_uid_t* uid);
bool ipc_port_compare(ipcport_t port, thread_uid_t uid);
