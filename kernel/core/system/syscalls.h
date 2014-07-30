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

/*! \file core/system/syscalls.h
 *  \date June 2014
 */

#pragma once

#include <arch.h>

void syscall_spawn();
void syscall_launch(uint16_t pid, uintptr_t entry);
void syscall_dispatch(uintptr_t entry);
void syscall_detach(uint16_t tid);
void syscall_kill(uint16_t pid);

void syscall_yield();

SYSCALL_TABLE = 
{
	{ syscall_spawn,    0 },
	{ syscall_launch,   2 },
	{ syscall_dispatch, 1 },
	{ syscall_detach,   1 },
	{ syscall_kill,     1 },

	{ syscall_yield,    0 },

	0
};
