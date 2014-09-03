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

#include "callback.h"
#include <sw/instr/misc.h>
#include <sw/int/irq.h>
#include <multitask/scheduler.h>
#include <debug/log.h>
#include <debug/error.h>
#include <stddef.h>

static struct
{
	int_callback_t handle;
	int_callback_t eoi_handle;
} callback_pairs[ISR_MAX] = {{0}};

static int_frame_t* curr_frame = NULL;

void int_callback_common(int_frame_t* frame);

//! Set a callback for the given ISR, as the main handle or the EOI.
/*! The function will warn if the ISR already has a callback assigned. */
void int_callback_set(isr_t isr, bool eoi, int_callback_t handle)
{
	int_callback_t* target;
	if (eoi)
		target = &(callback_pairs[isr].eoi_handle);
	else
		target = &(callback_pairs[isr].handle);

	if (*target && handle)
		dtrace("Warning: ISR%i already had the specified callback.", isr);

	*target = handle;
}

//! Get the register state before the interrupt occurred.
/*! Only valid during an interrupt (when in a callback). */
int_frame_t* int_callback_frame_get()
{
	dassert(curr_frame);
	return curr_frame;
}

// The interrupt stubs call this function to dispatch to the callbacks.
void int_callback_common(int_frame_t* frame)
{
	isr_t isr = frame->int_num;
	curr_frame = frame;

	if (callback_pairs[isr].handle)
		callback_pairs[isr].handle(isr, irq_from_isr(isr));
	if (callback_pairs[isr].eoi_handle)
		callback_pairs[isr].eoi_handle(isr, irq_from_isr(isr));

	curr_frame = NULL;
}
