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

#include "pit.h"
#include <sw/instr/misc.h>
#include <debug/error.h>
#include <spec/pit8253/io.h>

//! Setup a PIT timer with a given frequency and output mode.
void pit_set(PIT_TIMER timer, size_t freq, PIT_OUTMODE mode)
{
	dassert(freq);

	// Set up the command with some defaults + the params.
	pit_command_t cmd = {0};
	cmd.count_mode  = PIT_CNTMODE_BIN;
	cmd.output_mode = mode;
	cmd.access_mode = PIT_ACCESS_RW_BOTH;
	cmd.channel     = timer;

	pit_write(cmd, freq);
}

//! Put the CPU in an idle loop until the PIT one-shots.
void pit_sleep(size_t msec)
{
	// Use TIMER2 for sleeping.
	size_t freq = 1000 / msec;
	pit_set(PIT_TIMER2, freq, PIT_OUTMODE_ONESHOT);

	// Toggle the CH2 gate to start the one-shot.
	pit_control_t ctl = pit_control_read();
	ctl.gate_ch2 = 0;
	pit_control_write(ctl);
	ctl.gate_ch2 = 1;
	pit_control_write(ctl);

	// Wait until the output is high (the count finishes).
	/* FIXME: Is volatile needed here? */
	while (pit_control_read().out_ch2 == 0)
		pause();
}
