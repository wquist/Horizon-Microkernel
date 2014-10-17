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

/*! \file core/system/internal.h
 *  \date October 2014
 */

#pragma once

#include <multitask/thread.h>
#include <horizon/types.h>
#include <horizon/msg.h>

void internal_tkill(thread_uid_t uid);
void internal_ksend(ipcport_t port, msgdata_t code, msgdata_t args[MSG_ARGC]);
