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

/*! \file util/bmstack.h
 *  \date June 2014
 */

#pragma once

#include <util/bitmap.h>
#include <stddef.h>
#include <stdbool.h>

//! Keep a linked list of singular bitmaps with available bits.
typedef struct bmstack_entry bmstack_entry_t;
struct bmstack_entry
{
	bmstack_entry_t* next;
	bitmap_t bitmap;
};

//! A bmstack points to the base index and the next free bitmap.
typedef struct bmstack bmstack_t;
struct bmstack
{
	bmstack_entry_t* base;
	bmstack_entry_t* head;
};

#define BMSTACK_SIZE(x) (BITMAP_LENGTH(x) * (sizeof(bmstack_entry_t)))

void bmstack_init(bmstack_t* bms, void* base);
void bmstack_link(bmstack_t* bms, size_t items);

void bmstack_set(bmstack_t* bms, size_t index);
void bmstack_clear(bmstack_t* bms, size_t index);
bool bmstack_test(bmstack_t* bms, size_t index);

void bmstack_set_all(bmstack_t* bms, size_t items);
void bmstack_clear_all(bmstack_t* bms, size_t items);

long bmstack_find_and_set(bmstack_t* bms);
