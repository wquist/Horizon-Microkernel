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

/*! \file core/multitask/process.h
 *  \date September 2014
 */

#pragma once

#include <arch.h>
#include <multitask/thread.h>
#include <util/bitmap.h>
#include <horizon/types.h>
#include <stddef.h>
#include <stdint.h>

//! System-wide limits for processes and threads.
/*! Ideally, the maxes together should be representable in 16-bits. */
#define PROCESS_MAX 1024
#define PROCESS_THREAD_MAX  64
#define PROCESS_MESSAGE_MAX 1024

//! A process control block.
typedef struct process process_t;
struct process
{
	pid_t   pid;
	uint8_t version;
	pid_t   parent;

	size_t priv;

	uintptr_t entry;
	paging_pas_t* addr_space;

	struct
	{
		size_t   count;
		bitmap_t bitmap[BITMAP_LENGTH(PROCESS_MESSAGE_MAX)];
	} msg_info;

	struct
	{
		size_t   count;
		bitmap_t bitmap[BITMAP_LENGTH(PROCESS_THREAD_MAX)];

		uint8_t versions[PROCESS_THREAD_MAX];
	} thread_info;

	message_t* messages;
	thread_t*  threads;
};

void process_init();

pid_t process_new(pid_t ppid, uintptr_t entry);
void  process_kill(pid_t pid);

process_t* process_get(pid_t pid);
