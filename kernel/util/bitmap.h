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

/*! \file util/bitmap.h
 *  \date June 2014
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef uintptr_t bitmap_t;

#define BITMAP_BITS (8 * sizeof(bitmap_t))
#define BITMAP_LENGTH(x) ((x) / BITMAP_BITS)

static inline size_t bitmap_size(size_t items) { return (BITMAP_LENGTH(items) * sizeof(bitmap_t)); }

void bitmap_set(bitmap_t* bm, size_t index);
void bitmap_clear(bitmap_t* bm, size_t index);
bool bitmap_test(const bitmap_t* bm, size_t index);

void bitmap_set_all(bitmap_t* bm, size_t items);
void bitmap_clear_all(bitmap_t* bm, size_t items);

size_t bitmap_find_and_set(bitmap_t* bm, size_t items);
