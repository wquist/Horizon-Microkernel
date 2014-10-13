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

#include "isr.h"
#include <util/bitmap.h>
#include <debug/log.h>
#include <debug/error.h>

// One 16-bit bitmap for each priority level.
static uint16_t priority_levels[ISR_MAX / sizeof(uint16_t)] = {0};

//! Reserve a specific ISR (prevent it from being allocated).
void isr_reserve(isr_t isr)
{
	bool isset = bitmap_test((bitmap_t*)priority_levels, isr);
	dassert(!isset);

	bitmap_set((bitmap_t*)priority_levels, isr);
}

//! Get any ISR based on the specified priority level.
/*! Can return lower priorities if the given level has none free. */
isr_t isr_alloc(ISR_PRIORITY pri)
{
	dassert(pri < ISR_PRIORITY_LEVELS);
	bitmap_t* pri_bm = (bitmap_t*)&(priority_levels[pri]);
	size_t max_slots = (ISR_PRIORITY_LEVELS - pri) * 16;

	long isr = bitmap_find_and_set(pri_bm, max_slots);
	dassert(isr != -1);

	// The resulting ISR's number is relative to the given priority,
	isr += (pri * 16); //< so adjust to account for the ISRs before it.
	dtrace("Allocated ISR%i, priority %i. (Wanted priority %i)", isr, isr/16, pri);

	return isr;
}

//! Free an ISR to be allocated elsewhere.
void isr_free(isr_t isr)
{
	bitmap_clear((bitmap_t*)priority_levels, isr);
}
