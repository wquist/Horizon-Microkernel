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

#include "bmstack.h"
#include <debug/error.h>
#include <util/addr.h>
#include <memory.h>

//! Initialize a bmstack by setting its base.
void bmstack_init(bmstack_t* bms, void* base)
{
	bms->base = (bmstack_entry_t*)base;
	bms->head = NULL;
}

//! Link each bitmap entry together for use in find_and_set.
void bmstack_link(bmstack_t* bms, size_t items)
{
	bmstack_entry_t* prev = NULL;
	bmstack_entry_t* curr;

	for (size_t i = 0; i != BITMAP_LENGTH(items); ++i, prev = curr)
	{
		bmstack_entry_t* curr = &(bms->base[i]);
		curr->next = NULL;

		// If this singular bitmap is full...
		if (~(curr->bitmap) == 0)
			continue; //< Do not add it to the linked list.

		if (!prev) 
			bms->head = curr; //< Start of the linked list.
		else 
			prev->next = curr; //< Another link.
	}
}

//! Set an individual bit in the bmstack with a linear index.
void bmstack_set(bmstack_t* bms, size_t index)
{
	dassert(bms->base);
	size_t i   = index / BITMAP_BITS;
	size_t bit = index % BITMAP_BITS;

	bmstack_entry_t* entry = &(bms->base[i]);
	entry->bitmap |= (1 << bit);

	// If bmstack_link has been called and this bitmap is now full,
	if (entry == bms->head && ~(entry->bitmap) == 0)
		bms->head = entry->next; //< Remove it from the linked list.
}

//! Clear a bit in the bmstack.
void bmstack_clear(bmstack_t* bms, size_t index)
{
	dassert(bms->base);
	size_t i   = index / BITMAP_BITS;
	size_t bit = index % BITMAP_BITS;

	bmstack_entry_t* entry = &(bms->base[i]);
	bool was_full = (~(entry->bitmap) == 0);
	entry->bitmap &= ~(1 << bit);

	// If the stack has been linked and this bitmap is not part of the list,
	if (bms->head && was_full)
	{
		// Insert it at the head of the list.
		entry->next = bms->head;
		bms->head   = entry;
	}
}

//! Test if a bit is currently set in the bmstack.
bool bmstack_test(bmstack_t* bms, size_t index)
{
	dassert(bms->base);
	size_t i   = index / BITMAP_BITS;
	size_t bit = index % BITMAP_BITS;

	bmstack_entry_t* entry = &(bms->base[i]);
	return (entry->bitmap & (1 << bit));
}

//! Set 'items' bits to 1 in the bmstack.
void bmstack_set_all(bmstack_t* bms, size_t items)
{
	dassert(bms->base);
	memset(bms->base, 0xFF, bmstack_size(items));
}

//! Set 'items' bits to 0.
void bmstack_clear_all(bmstack_t* bms, size_t items)
{
	dassert(bms->base);
	memset(bms->base, 0x00, bmstack_size(items));
}

size_t bmstack_find_and_set(bmstack_t* bms)
{
	dassert(bms->head);

	// Find the least significant 0 bit.
	int bit = __builtin_ffsl(~(bms->head->bitmap)) - 1;
	if (bit < 0) return -1; //< Problem; head should always have free bit.

	uintptr_t base = (uintptr_t)(bms->base);
	uintptr_t head = (uintptr_t)(bms->head);
	size_t i = addr_to_index(base, sizeof(bmstack_entry_t), head); 
	size_t index  = (i * BITMAP_BITS) + bit;

	bms->base[i].bitmap |= (1 << bit);
	return index; 
}
