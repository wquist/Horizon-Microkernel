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

/*! \file core/multitask/thread.h
 *  \date July 2014
 */

#pragma once

#include <arch.h>
#include <stdint.h>

#define THREAD_MAX (2<<15)
#define THREAD_BLOCK_SIZE 4096

typedef struct thread thread_t;
struct thread
{
	uint16_t tid, lid;
	uint16_t owner;

	uintptr_t entry;
	int_frame_t frame;

	struct
	{
		struct { uint16_t prev, uint16_t next; } queue;
		uint8_t timeslice;
		// FIXME: thread state.
	} sched;

	// FIXME: message queue.
};

//
