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

/*! \file core/ipc/message.h
 *  \date July 2014
 */

#pragma once

#include <horizon/msg.h>
#include <horizon/types.h>
#include <stdint.h>
#include <stdbool.h>

enum message_flags
{
	MESSAGE_FLAG_PAYLOAD = (1 << 0),
};

// FIXME: Kind of an odd size and wasted space.
typedef struct message message_t;
struct __packed message
{
	ipcchan_t channel;

	pid_t sender;
	ipcsrc_t source;

	//! Maintain a linked list for the queue.
	/*! Current queue max is 128 so 256 should be plenty. */
	uint8_t next;
	uint8_t flags;
	uint16_t _reserved; //< Needed to space to dword multiple.

	msgarg_t code, arg;
};

void message_send(ipcchan_t from, tid_t to, struct msg* info, bool head);
uint8_t message_recv(tid_t src, struct msg* dest);
uint8_t message_peek(tid_t src, ipcchan_t* from);

bool message_find(tid_t src, ipcchan_t search);
