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
#include <ipc/target.h>
#include <util/bitmap.h>
#include <debug/error.h>
#include <horizon/ipc.h>
#include <stddef.h>
#include <string.h>

//! Send a message from with the given info to a TID.
/*! 'head' determines if the message is pushed to the front or back of the queue. */
void message_send(ipcchan_t from, tid_t to, struct msg* info, bool head)
{
	dassert(info);

	// Make sure the thread exists and has room in its queue.
	thread_t* thread = thread_get(to);
	dassert(thread);
	dassert(thread->messages.count < THREAD_MESSAGE_MAX);

	uint8_t slot = bitmap_find_and_set(thread->messages.bitmap, THREAD_MESSAGE_MAX);
	dassert(slot != -1);

	message_t* target = &(thread->messages.slots[slot]);
	memset(target, 0, sizeof(message_t));

	thread_t* sender = thread_get(from);
	if (sender) //< Needed in case the sender is the kernel (0).
	{
		process_t* owner = process_get(sender->owner);
		dassert(owner);

		target->channel = ((ipcchan_t)(sender->version) << 16) | from;
		target->sender  = ((ipcchan_t)(owner->version)  << 16) | owner->pid;
	}

	target->code = info->code;
	target->arg  = info->arg;

	if (info->payload.buf)
		target->flags |= MESSAGE_FLAG_PAYLOAD;

	target->next = -1; //< Mark the end of the linked list.
	if (thread->messages.count != 0)
	{
		if (head) //< Put this message at the front of the list.
		{
			target->next = thread->messages.head;
			thread->messages.head = slot;
		}
		else
		{
			// Tail is always valid when there are messages in the queue (count > 0).
			message_t* last = &(thread->messages.slots[thread->messages.tail]);

			last->next = slot;
			thread->messages.tail = slot;
		}
	}
	else
	{
		// The only message in queue is the head and tail.
		thread->messages.head = slot;
		thread->messages.tail = slot;
	}

	++(thread->messages.count);
}

//! Copy message info to the given struct and remove it from the queue.
/*! The destination message can be null; the msg will still be removed. */
uint8_t message_recv(tid_t src, struct msg* dest)
{
	// A message must be in queue for recv to work.
	thread_t* thread = thread_get(src);
	dassert(thread);
	dassert(thread->messages.count);

	// Head is also valid under the same conditions as tail.
	message_t* head = &(thread->messages.slots[thread->messages.head]);
	bitmap_clear(thread->messages.bitmap, thread->messages.head);

	if (dest)
	{
		dest->from.channel = head->channel;
		dest->from.sender  = head->sender;

		dest->code = head->code;
		dest->arg  = head->arg;
	}

	// Update the head and the tail.
	if (thread->messages.tail == thread->messages.head)
		thread->messages.tail = -1;
	thread->messages.head = head->next; //< 'next' could be msg or -1.

	--(thread->messages.count);
	return head->flags;
}

//! Get the message sender and flags from a queue head.
/*! FIXME: Consolidate with message_recv? */
uint8_t message_peek(tid_t src, ipcchan_t* from)
{
	dassert(from);

	thread_t* thread = thread_get(src);
	dassert(thread);
	dassert(thread->messages.count);

	message_t* head = &(thread->messages.slots[thread->messages.head]);

	*from = head->channel;
	return head->flags;
}

//! Search for a message from the given sender and put it at the queue head.
bool message_find(tid_t src, ipcchan_t search)
{
	thread_t* thread = thread_get(src);
	dassert(thread);

	// Bail out early if there is nothing to search.
	if (thread->messages.count == 0)
		return false;
	
	uint8_t prev = -1;
	uint8_t curr = thread->messages.head;
	while (curr != -1)
	{
		message_t* msg = &(thread->messages.slots[curr]);
		if (ipc_message_compare(search, msg))
		{
			if (prev != -1) //< The message is in the middle or at the end.
				thread->messages.slots[prev].next = msg->next;
			else // The message was the head.
				thread->messages.head = msg->next;

			// Was the message also the tail?
			if (thread->messages.tail == curr)
				thread->messages.tail = prev;

			// FIXME: Do not just replace the msg at head.
			msg->next = thread->messages.head;
			thread->messages.head = curr;
			if (thread->messages.tail == -1)
				thread->messages.tail = curr;

			return true;
		}

		prev = curr;
		curr = msg->next;
	}

	return false;
}
