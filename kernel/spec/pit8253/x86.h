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

/*! \file spec/pit8253/x86.h
 *  \date July 2014
 */

#pragma once

#include <arch.h>
#include <spec/pit8253.h>
#include <stddef.h>

//! The x86 port connections for the PIT.
typedef enum pit_port PIT_PORT;
enum pit_port
{
	PIT_PORT_COMMAND = 0x43, //!< Write only.
	PIT_PORT_CONTROL = 0x61,

	PIT_PORT_CHANNEL0 = 0x40,
	PIT_PORT_CHANNEL1 = 0x41,
	PIT_PORT_CHANNEL2 = 0x42
};

//! Read the upper 4 bits from the control port.
static inline pit_control_t pit_control_read()
{
	pit_control_t res = { .raw = inb(PIT_PORT_CONTROL) };
	return res;
}

//! Write the lower 4 bits to the control port.
static inline void pit_control_write(pit_control_t ctl)
{
	// Only the first 4 bits are writable.
	outb(PIT_PORT_CONTROL, ctl.raw & 0xF);
}

void pit_write(pit_command_t cmd, size_t freq);

uint16_t pit_count_read(PIT_TIMER timer);
pit_status_t pit_status_read(PIT_TIMER timer);
