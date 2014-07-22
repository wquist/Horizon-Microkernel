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

#include "io.h"
#include <arch.h>

//! Write a raw command + frequency to a PIT timer.
void pit_write(pit_command_t cmd, size_t freq)
{
	outb(PIT_PORT_COMMAND, cmd.raw);

	// Only write the command?
	if (!freq)
		return;

	uint8_t  channel = PIT_PORT_CHANNEL0 + cmd.channel;
	uint16_t divider = PIT_BASE_FREQ / freq;

	// If the access mode is high or low, write only that byte.
	/* Otherwise, the low byte is written, then the high byte. */
	if (cmd.access_mode & PIT_ACCESS_RW_LOW)
		outb(channel, divider & 0xFF);
	if (cmd.access_mode & PIT_ACCESS_RW_HIGH)
		outb(channel, (divider >> 8) & 0xFF);
}

//! Read the current value of the specified timer.
/*! The count is the number of ticks before an INT is fired. */
uint16_t pit_count_read(PIT_TIMER timer)
{
	uint16_t port = PIT_PORT_CHANNEL0 + timer;

	pit_status_t status = pit_status_read(timer);
	// If the access is only high or low, the count is <= 0xFF.
	if (status.access_mode != PIT_ACCESS_RW_BOTH)
		return (uint16_t)inb(port);

	// Otherwise, the PIT must be latched to read twice.
	pit_command_t cmd = {0};
	cmd.access_mode = PIT_ACCESS_LATCH;
	cmd.channel     = timer;

	outb(PIT_PORT_COMMAND, cmd.raw);

	// Now read both values.
	uint16_t res = (uint16_t)inb(port); //< Get the low first.
	res |= (uint16_t)inb(port) << 8; //< Shift in the high next.
	return res;
}

//! Get the status of the specified timer.
pit_status_t pit_status_read(PIT_TIMER timer)
{
	// Set up a status request.
	pit_command_t cmd = {0};
	cmd.readback_t0 = (timer == PIT_TIMER0);
	cmd.readback_t1 = (timer == PIT_TIMER1);
	cmd.readback_t2 = (timer == PIT_TIMER2);
	cmd.no_count    =  true;
	cmd.always_1    =  3; //< 0b11.

	outb(PIT_PORT_COMMAND, cmd.raw);

	pit_status_t status = { .raw = inb(PIT_PORT_CHANNEL0 + timer) };
	return status;
}
