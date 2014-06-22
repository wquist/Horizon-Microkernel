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

#include "bitmap.h"
#include <memory.h>

//! Set a bit in the bitmap at the given index.
void bitmap_set(bitmap_t* bm, size_t index)
{
	size_t i   = index / BITMAP_BITS;
	size_t bit = index % BITMAP_BITS;

	bm[i] |= (1 << bit);
}

//! Clear the given bit in the bitmap.
void bitmap_clear(bitmap_t* bm, size_t index)
{
	size_t i   = index / BITMAP_BITS;
	size_t bit = index % BITMAP_BITS;

	bm[i] &= ~(1 << bit);
}

//! Check if the bit for the given index is set.
bool bitmap_test(const bitmap_t* bm, size_t index)
{
	size_t i   = index / BITMAP_BITS;
	size_t bit = index % BITMAP_BITS;

	return (bm[i] & (1 << bit));
}

//! Set 'items' bits to 1.
void bitmap_set_all(bitmap_t* bm, size_t items)
{
	memset(bm, 0xFF, BITMAP_SIZE(items));
}

//! Clear 'items' bits to 0.
void bitmap_clear_all(bitmap_t* bm, size_t items)
{
	memset(bm, 0x00, BITMAP_SIZE(items));
}

//! Find the first clear bit in the bitmap, set it, and return the index.
size_t bitmap_find_and_set(bitmap_t* bm, size_t items)
{
	size_t i = 0;
	for (; ~(bm[i]) == 0; ++i) //< Compare with inverse; ptr size varies per on platform.
	{
		if (i >= items)
			return -1;
	}

	// Find the least significant zero bit.
	int bit = __builtin_ffsl(~(bm[i])) - 1; //< ffsl return lszb + 1.
	size_t index = (i * BITMAP_BITS) + bit;

	bm[i] |= (1 << bit);
	return index;
}
