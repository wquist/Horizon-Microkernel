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
#include <horizon/types.h>
#include <horizon/msg.h>
#include <stddef.h>
#include <stdint.h>

void syscall_spawn();
void syscall_launch(pid_t pid, uintptr_t entry);
void syscall_dispatch(uintptr_t entry, uintptr_t stack);
void syscall_detach(tid_t tid);
void syscall_kill(pid_t pid);

void syscall_yield();
void syscall_wait(ipcdst_t sender);

void syscall_vmap(uintptr_t dest, size_t size);
void syscall_pmap(uintptr_t dest, uintptr_t src, size_t size);
void syscall_unmap(uintptr_t addr, size_t size);

void syscall_send(struct msg* src);
void syscall_recv(struct msg* dest);
void syscall_peek();
void syscall_drop(struct msg* info);

SYSCALL_TABLE = 
{
	{ syscall_spawn,    0 },
	{ syscall_launch,   2 },
	{ syscall_dispatch, 2 },
	{ syscall_detach,   1 },
	{ syscall_kill,     1 },

	{ syscall_yield,    0 },
	{ syscall_wait,     1 },

	{ syscall_vmap,     2 },
	{ syscall_pmap,     3 },
	{ syscall_unmap,    2 },

	{ syscall_send,     1 },
	{ syscall_recv,     1 },
	{ syscall_peek,     0 },
	{ syscall_drop,     1 },

	0
};
