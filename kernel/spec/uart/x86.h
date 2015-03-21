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

/*! \file spec/uart/x86.h
 *  \date March 2015
 */

#pragma once

#include <spec/uart.h>

//! The I/O ports used by UART.
/*! The higher number ports are less likely to actually exist. */
typedef enum uart_port UART_PORT;
enum uart_port
{
	UART_COM1 = 0x3F8,
	UART_COM2 = 0x2F8,
	UART_COM3 = 0x3E8,
	UART_COM4 = 0x2E8
};

//! The I/O port offsets for the UART registers.
typedef enum uart_reg UART_REG;
enum uart_reg
{
	UART_REG_DATA    = 0x0,
	UART_REG_IER     = 0x1,
	UART_REG_BAUDL   = 0x0,
	UART_REG_BAUDH   = 0x0,
	UART_REG_IIR     = 0x2,
	UART_REG_FCR     = 0x2,
	UART_REG_LCR     = 0x3,
	UART_REG_MCR     = 0x4,
	UART_REG_LSR     = 0x5,
	UART_REG_MSR     = 0x6,
	UART_REG_SCRATCH = 0x7
};

void uart_write(UART_PORT port, UART_REG reg, uint8_t data);
uint8_t uart_read(UART_PORT port, UART_REG reg);

void uart_baud_set(UART_PORT port, uint16_t divisor);
