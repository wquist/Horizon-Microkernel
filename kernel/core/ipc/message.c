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
#include <ipc/port.h>
#include <util/bitmap.h>
#include <debug/error.h>
#include <memory.h>
#include <stdbool.h>

// Represents the end of a linked list.
#define NULL_INDEX (PROCESS_MESSAGE_MAX)

//! Place the given message into the target processes message queue.
/*! The message is placed at the front of the queue if 'head' is 'true'. */
void message_add(thread_uid_t uid, ipcport_t from, struct msg* info, bool head)
{
	dassert(info);

	process_t* owner = process_get(uid.pid);
	dassert(owner);
	dassert(owner->msg_info.count <= PROCESS_MESSAGE_MAX);

	size_t index = bitmap_find_and_set(owner->msg_info.bitmap, PROCESS_MESSAGE_MAX);
	dassert(index != -1);

	message_t* target = &(owner->messages[index]);
	memset(target, 0, sizeof(message_t));

	target->from = from;
	target->code = info->code;
	target->arg  = info->arg;
	target->payload_flag = (info->payload.size > 0);

	// The head and tail of the queue are stored per-thread.
	thread_t* thread = thread_get(uid);
	dassert(thread);

	// Update the linked list for the target thread.
	if (head) //< Prepend the message.
	{
		// The queue is currently empty for this TID.
		if (thread->msg_info.head == NULL_INDEX)
			thread->msg_info.tail = index;

		target->next = thread->msg_info.head;
		thread->msg_info.head = index;
	}
	else //< Or append it.
	{
		// The tail == NULL_INDEX also means the queue is empty.
		if (thread->msg_info.tail == NULL_INDEX)
			thread->msg_info.head = index;
		else
			owner->messages[thread->msg_info.tail].next = index;

		target->next = NULL_INDEX;
		thread->msg_info.tail = index;
	}

	owner->msg_info.count  += 1;
	thread->msg_info.count += 1;
}

//! Get the message at the head of the queue and remove it.
/*! Returns true if the message was associated with a payload. */
bool message_remove(thread_uid_t uid, struct msg* dest)
{
	process_t* owner = process_get(uid.pid);
	dassert(owner);

	// A message must be queued for the given thread.
	thread_t* thread = thread_get(uid);
	dassert(thread);
	dassert(thread->msg_info.count);

	// Since a message exists, the head is definitely not NULL_INDEX.
	message_t* source = &(owner->messages[thread->msg_info.head]);
	bitmap_clear(owner->msg_info.bitmap, thread->msg_info.head);

	// Actually getting the message is optional.
	if (dest)
	{
		dest->from = source->from;
		dest->code = source->code;
		dest->arg  = source->arg;
	}

	// Update the linked list.
	thread->msg_info.head = source->next;
	if (thread->msg_info.head == NULL_INDEX) //< Message was the last in queue.
		thread->msg_info.tail = NULL_INDEX;

	owner->msg_info.count  -= 1;
	thread->msg_info.count -= 1;

	return source->payload_flag;
}

//! Get the sender and payload info of the head message without dequeue-ing it.
bool message_peek(thread_uid_t uid, ipcport_t* from)
{
	process_t* owner = process_get(uid.pid);
	dassert(owner);

	thread_t* thread = thread_get(uid);
	dassert(thread);
	dassert(thread->msg_info.count);

	message_t* source = &(owner->messages[thread->msg_info.head]);

	*from = source->from;
	return source->payload_flag;
}

//! Search for a message to bring to the head of the queue.
/*! Returns 'true' if a message was moved to the front. */
bool message_find(thread_uid_t uid, ipcport_t search)
{
	process_t* owner = process_get(uid.pid);
	dassert(owner);

	thread_t* thread = thread_get(uid);
	dassert(thread);

	uint16_t prev = NULL_INDEX;
	uint16_t curr = thread->msg_info.head;
	while (curr != NULL_INDEX)
	{
		message_t* msg = &(owner->messages[curr]);
		if (ipc_port_compare(search, uid))
		{
			if (prev == NULL_INDEX) //< This message is already the head.
				return true;

			// Remove the message from its position in the list.
			/* At this point, at least 2 messages are in queue. */
			owner->messages[prev].next = msg->next;
			if (thread->msg_info.tail == curr)
				thread->msg_info.tail = prev;

			// Replace the message at the head.
			msg->next = thread->msg_info.head;
			thread->msg_info.head = curr;

			return true;
		}

		prev = curr;
		curr = msg->next;
	}

	return false;
}
