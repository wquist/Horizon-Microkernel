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
 *  \date September 2014
 */

#pragma once

#include <arch.h>
#include <horizon/types.h>
#include <stddef.h>
#include <stdint.h>

typedef enum thread_state THREAD_STATE;
enum thread_state
{
	THREAD_STATE_NEW,
	THREAD_STATE_ACTIVE,
	THREAD_STATE_SENDING,
	THREAD_STATE_WAITING,
	THREAD_STATE_OLD
};

typedef union thread_uid thread_uid_t;
union thread_uid
{
	struct __packed
	{
		uint16_t pid : 10;
		uint16_t tid : 6;
	};
	uint16_t raw;
};

typedef struct thread thread_t;
struct thread
{
	tid_t   tid;
	uint8_t version;
	pid_t   owner;

	THREAD_STATE state;
	task_info_t task;

	struct
	{
		size_t count;
		uint16_t head, tail;
	} msg_info;

	struct
	{
		size_t timeslice;
		thread_uid_t next, prev;
	} sched_info;

	struct
	{
		//
	} syscall_info;
};

thread_uid_t thread_new(pid_t pid, uintptr_t entry);
void  thread_kill(thread_uid_t uid);

thread_t* thread_get(thread_uid_t uid);
