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

#include "message.h"
#include <multitask/process.h>
#include <util/bitmap.h>
#include <debug/error.h>
#include <stddef.h>
#include <string.h>

void message_send(uint16_t dest, message_t* msg)
{
	dassert(msg);

	// Make sure the thread exists and has room in its queue.
	thread_t* thread = thread_get(dest);
	dassert(thread);
	dassert(thread->messages.count < THREAD_MESSAGE_MAX);

	uint8_t slot = bitmap_find_and_set(thread->messages.bitmap, THREAD_MESSAGE_MAX);
	dassert(slot != -1);

	message_t* tail = &(thread->messages.slots[slot]);
	memcpy(tail, msg, sizeof(message_t));

	tail->next = -1; //< Mark the end of the linked list.
	if (thread->messages.count != 0)
	{
		// Tail is always valid when there are messages in the queue (count > 0).
		message_t* last = &(thread->messages.slots[thread->messages.tail]);

		last->next = slot;
		thread->messages.tail = slot;
	}
	else
	{
		// The only message in queue is the head and tail.
		thread->messages.head = slot;
		thread->messages.tail = slot;
	}

	++(thread->messages.count);
}

void message_recv(uint16_t src, message_t* msg)
{
	// A message must be in queue for recv to work.
	thread_t* thread = thread_get(src);
	dassert(thread);
	dassert(thread->messages.count);

	// Head is also valid under the same conditions as tail.
	message_t* head = &(thread->messages.slots[thread->messages.head]);
	memcpy(msg, head, sizeof(message_t));

	thread->messages.head = head->next;

	--(thread->messages.count);
}
