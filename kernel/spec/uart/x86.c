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

#include "x86.h"
#include <arch.h>
#include <stdbool.h>

//! Write a byte to one of the UART registers on the given port.
void uart_write(UART_PORT port, UART_REG reg, uint8_t data)
{
	outb(port + reg, data);
}

//! Read a byte from the given UART port.
uint8_t uart_read(UART_PORT port, UART_REG reg)
{
	return inb(port + reg);
}

void uart_baud_set(UART_PORT port, uint16_t divisor)
{
	// Store the current LCR state.
	uart_lcr_t lcr = { .raw = uart_read(port, UART_REG_LCR) };

	// Enable DLAB to set baud divisor.
	lcr.DLAB = true;
	uart_write(port, UART_REG_LCR, lcr.raw);

	// Set the low and high bytes of the divisor.
	uart_write(port, UART_REG_BAUDL, (divisor) & 0xFF);
	uart_write(port, UART_REG_BAUDH, (divisor >> 8) & 0xFF);

	// Restore LCR state.
	lcr.DLAB = false;
	uart_write(port, UART_REG_LCR, lcr.raw);
}
