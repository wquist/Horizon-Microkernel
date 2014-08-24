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
 *  \date July 2014
 */

#pragma once

#include <arch.h>
#include <ipc/message.h>
#include <util/bitmap.h>
#include <horizon/types.h>
#include <horizon/shm.h>
#include <stddef.h>
#include <stdint.h>

#define PROCESS_MAX (1<<15)
#define PROCESS_BLOCK_SIZE 4096
//! The number of threads an individual PID can own.
/*! A bitmap with this many bits is stored in the PCB. */
#define PROCESS_THREAD_MAX 1024

#define THREAD_MAX (1<<15)
#define THREAD_BLOCK_SIZE 4096
//! The number of messages a TID can queue.
#define THREAD_MESSAGE_MAX 128

typedef enum thread_state THREAD_STATE;
enum thread_state
{
	THREAD_STATE_NEW,
	THREAD_STATE_ACTIVE,
	THREAD_STATE_SENDING,
	THREAD_STATE_WAITING,
	THREAD_STATE_OLD,
};

//! A process control block (PCB).
typedef struct process process_t;
struct process
{
	pid_t pid, parent;
	size_t priv;

	uintptr_t entry;
	paging_pas_t* addr_space;

	struct
	{
		// FIXME: Macro this/put it in another file.
		struct shm shm_slots[16];
		uint8_t shm_next;
	} call_data;

	struct
	{
		size_t count;
		uint16_t slots[PROCESS_THREAD_MAX];
		bitmap_t bitmap[BITMAP_LENGTH(PROCESS_THREAD_MAX)];
	} threads;
};

//! A thread control block (TCB).
typedef struct thread thread_t;
struct thread
{
	tid_t tid, lid;
	pid_t owner;

	task_info_t task;

	struct
	{
		struct { tid_t prev, next; } queue;
		uint8_t timeslice;
		THREAD_STATE state;
	} sched;

	struct
	{
		ipcdst_t wait_for;

		uintptr_t payload_addr;
		size_t payload_size;
	} call_data;

	struct
	{
		uint8_t head, tail;
		size_t count;
		message_t slots[THREAD_MESSAGE_MAX];
		bitmap_t bitmap[BITMAP_LENGTH(THREAD_MESSAGE_MAX)];
	} messages;
};

void process_init();
void thread_init(); //!< Called internally.

pid_t process_new(pid_t ppid, uintptr_t entry);
void process_kill(pid_t pid);

tid_t thread_new(pid_t pid, uintptr_t entry);
void thread_kill(tid_t tid);

process_t* process_get(pid_t pid);
thread_t* thread_get(tid_t tid);
