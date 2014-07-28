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

/*! \file arch/i586/sw/int/syscall.h
 *  \date July 2014
 */

#pragma once

#include <sw/int/frame.h>
#include <sw/int/callback.h>
#include <stddef.h>
#include <stdint.h>

#define SYSCALL_ISR 0x95

typedef struct syscall syscall_t;
struct syscall
{
	union
	{
		void* raw;
		void (*arg0)();
		void (*arg1)(uintptr_t);
		void (*arg2)(uintptr_t, uintptr_t);
		void (*arg3)(uintptr_t, uintptr_t, uintptr_t);
	} function;

	size_t arg_count;
};

//! During an interrupt, read the current return value to be set on return to usermode.
static inline uintptr_t syscall_return_get() { return int_callback_frame_get()->EAX; }
//! During an interrupt, set the return value to the given value.
static inline void syscall_return_set(uintptr_t val) { int_callback_frame_get()->EAX = val; }

void syscall_init();
