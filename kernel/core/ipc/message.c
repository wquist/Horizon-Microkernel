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

#define MSGDEST_UID(x)  ((uint16_t)(x & 0xFFFF))
#define MSGDEST_TYPE(x) ((uint16_t)((x >> 16) & 0xFFFF))

//! Send a message from with the given info to a TID.
/*! 'head' determines if the message is pushed to the front or back of the queue. */
void message_send(tid_t from, tid_t to, struct msg* info, bool head)
{
	dassert(info);

	// Make sure the thread exists and has room in its queue.
	thread_t* thread = thread_get(to);
	dassert(thread);
	dassert(thread->messages.count < THREAD_MESSAGE_MAX);

	uint8_t slot = bitmap_find_and_set(thread->messages.bitmap, THREAD_MESSAGE_MAX);
	dassert(slot != -1);

	message_t* target = &(thread->messages.slots[slot]);
	target->sender = from;

	target->code = info->code;
	target->arg  = info->arg;
	target->data = info->data;

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
uint8_t message_recv(tid_t src, struct msg* dest)
{
	dassert(dest);

	// A message must be in queue for recv to work.
	thread_t* thread = thread_get(src);
	dassert(thread);
	dassert(thread->messages.count);

	// Head is also valid under the same conditions as tail.
	message_t* head = &(thread->messages.slots[thread->messages.head]);

	thread_t* sender = thread_get(head->sender);
	if (sender)
		dest->from = (sender->owner << 16) | (sender->tid);
	else
		dest->from = sender->tid;

	dest->code = head->code;
	dest->arg  = head->arg;
	dest->data = head->data;

	thread->messages.head = head->next;
	--(thread->messages.count);

	return head->flags;
}

//! Get the message sender and flags from a queue head.
/*! FIXME: Consolidate with message_recv? */
uint8_t message_peek(tid_t src, msgsrc_t* from)
{
	dassert(from);

	thread_t* thread = thread_get(src);
	dassert(thread);
	dassert(thread->messages.count);

	message_t* head = &(thread->messages.slots[thread->messages.head]);

	thread_t* sender = thread_get(head->sender);
	if (sender)
		*from = (sender->owner << 16) | (sender->tid);
	else
		*from = sender->tid;

	return head->flags;
}

//! Convert the msgdest type into a TID value.
uint16_t message_dest_get(msgdst_t dest)
{
	uint16_t uid = MSGDEST_UID(dest);
	switch (MSGDEST_TYPE(dest))
	{
		case MTOTID:
			return uid;
		case MTOPID:
			return process_get(uid)->threads.slots[0];
	}

	return 0;
}

//! Compare a msgdest type with a TID to see if they are equal.
bool message_dest_compare(msgdst_t dest, uint16_t caller)
{
	uint16_t uid = MSGDEST_UID(dest);
	if (uid == 0xFFFF)
		return true;

	thread_t* thread = thread_get(caller);
	switch (MSGDEST_TYPE(dest))
	{
		case MTOTID:
			return (thread->tid == uid);
		case MTOPID:
			return (thread->owner == uid);
	}

	return false;
}
