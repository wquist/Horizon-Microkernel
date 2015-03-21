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

#include "serial.h"
#include <sw/instr/misc.h>
#include <debug/log.h>

//! Initialize the given serial port with a baud of 2.4 kHz.
void serial_init(UART_PORT port)
{
	// Disable all interrupts.
	uart_write(port, UART_REG_IER, 0x0);

	uint16_t divisor = uart_divisor_get(2400);
	uart_baud_set(port, divisor);

	uart_lcr_t lcr = {0};
	lcr.word_size = UART_WORDSIZE_8;
	lcr.parity = UART_PARITY_NONE;
	// 8 bit, one stop bit, no parity, no set break.
	uart_write(port, UART_REG_LCR, lcr.raw);

	// Disable IRQs.
	uart_write(port, UART_REG_MCR, 0x0);

	uart_read(port, UART_REG_DATA); //< Clear any existing data.
	dtrace("Initialized serial port 0x%X.", port);
}

//! Write a byte to the given serial port.
void serial_write(UART_PORT port, uint8_t data)
{
	uart_lsr_t lsr;
	// Wait for ready status.
	while ((lsr.raw = uart_read(port, UART_REG_LSR)), !(lsr.empty_out))
		pause();

	uart_write(port, UART_REG_DATA, data);
}

//! Wait to receive a byte from the given serial port.
uint8_t serial_read(UART_PORT port)
{
	uart_lsr_t lsr;
	// Wait until a byte is available.
	while ((lsr.raw = uart_read(port, UART_REG_LSR)), !(lsr.data_ready))
		pause();

	return uart_read(port, UART_REG_DATA);
}
