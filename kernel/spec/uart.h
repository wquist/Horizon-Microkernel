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

/*! \file spec/uart.h
 *  \date March 2015
 */

#pragma once

#include <stdint.h>

//! UART clock frequency in Hz.
#define UART_FREQUENCY 115200

//! UART interrupt identifiers.
/*! Used in the Interrupt Identification Register. */
typedef enum uart_int UART_INT;
enum uart_int
{
	UART_INT_MODEM   = 0x0, //< Modem Status.
	UART_INT_DATAOUT = 0x1, //< Transmitter Holding Register Empty.
	UART_INT_DATAIN  = 0x2, //< Recevied Data Available.
	UART_INT_LINE    = 0x3, //< Line Status.
	UART_INT_TIMEOUT = 0x6  //< Timeout Pending.
};

//! UART FIFO status.
/*! Used in the Interrupt Identificaion Register. */
typedef enum uart_fifo UART_FIFO;
enum uart_fifo
{
	UART_FIFO_NONE     = 0x0,
	UART_FIFO_DISABLED = 0x2,
	UART_FIFO_ENABLED  = 0x3
};

//! UART trigger threshold in bytes.
typedef enum uart_trigger UART_TRIGGER;
enum uart_trigger
{
	UART_TRIGGER_1  = 0x0,
	UART_TRIGGER_4  = 0x1,
	UART_TRIGGER_8  = 0x2,
	UART_TRIGGER_14 = 0x3
};

//! UART word size in bits.
typedef enum uart_wordsize UART_WORDSIZE;
enum uart_wordsize
{
	UART_WORDSIZE_5 = 0x0,
	UART_WORDSIZE_6 = 0x1,
	UART_WORDSIZE_7 = 0x2,
	UART_WORDSIZE_8 = 0x3
};

//! UART parity mode.
typedef enum uart_parity UART_PARITY;
enum uart_parity
{
	UART_PARITY_NONE  = 0x0,
	UART_PARITY_ODD   = 0x1,
	UART_PARITY_EVEN  = 0x3,
	UART_PARITY_MARK  = 0x5, //< Parity always 1.
	UART_PARITY_SPACE = 0x7  //< Parity always 0.
};

//! The Interrupt Enable Register.
typedef union uart_ier uart_ier_t;
union uart_ier
{
	struct __packed
	{
		uint8_t data_in       : 1; //< See UART_INT_DATAIN.
		uint8_t data_out      : 1; //< See UART_INT_DATAOUT.
		uint8_t line_status   : 1; //< See UART_INT_LINE.
		uint8_t modem_status  : 1; //< See UART_INT_MODEM.
		uint8_t sleep_mode    : 1; //< Sleep Mode (16750 only, else reserved).
		uint8_t lowpower_mode : 1; //< Low Power Mode (16750 only, else reserved).
		uint8_t               : 2;
	};
	uint8_t raw;
};

//! The Interrupt Identification Register.
typedef union uart_iir uart_iir_t;
union uart_iir
{
	struct __packed
	{
		uint8_t pending     : 1; //< Interrupt pending.
		uint8_t interrupt   : 3; //< Interrupt statuses (see UART_INT).
		uint8_t             : 1;
		uint8_t fifo64      : 1; //< 64-byte FIFO (16750 only).
		uint8_t fifo_status : 2; //< FIFO status (see UART_FIFO).
	};
	uint8_t raw;
};

//! The FIFO Control Register.
typedef union uart_fcr uart_fcr_t;
union uart_fcr
{
	struct __packed
	{
		uint8_t enable    : 1; //< Enable FIFOs.
		uint8_t clear_in  : 1; //< Clear Receive FIFO.
		uint8_t clear_out : 1; //< Clear Transmit FIFO.
		uint8_t dma_mode  : 1;
		uint8_t           : 1;
		uint8_t fifo64    : 1; //< Enable 64-byte FIFO (16750 only).
		uint8_t trigger   : 1; //< FIFO Trigger Threshold (see UART_TRIGGER).
	};
	uint8_t raw;
};

//! The Line Control Register.
typedef union uart_lcr uart_lcr_t;
union uart_lcr
{
	struct __packed
	{
		uint8_t word_size  : 2; //< I/O word size (see UART_WORDSIZE).
		uint8_t multi_stop : 1; //< Use 1.5/2 stop bits instead of 1.
		uint8_t parity     : 3; //< Parity mode (see UART_PARITY).
		uint8_t set_break  : 1; //< Set Break Enable.
		uint8_t DLAB       : 1; //< Divisor Latch Access Bit.
	};
	uint8_t raw;
};

//! The Modem Control Register.
typedef union uart_mcr uart_mcr_t;
union uart_mcr
{
	struct __packed
	{
		uint8_t data_ready   : 1;
		uint8_t request_send : 1;
		uint8_t output_aux1  : 1; //< May be connected to RS-232 port.
		uint8_t output_aux2  : 1; //< Same as AUX1.
		uint8_t loopback     : 1;
		uint8_t autoflow_ctl : 1; //< Autoflow Control Enable (16750 only).
		uint8_t              : 2;
	};
	uint8_t raw;
};

//! The Line Status Register.
typedef union uart_lsr uart_lsr_t;
union uart_lsr
{
	struct __packed
	{
		uint8_t data_ready  : 1;
		uint8_t overrun_err : 1;
		uint8_t parity_err  : 1;
		uint8_t framing_err : 1;
		uint8_t break_int   : 1;
		uint8_t empty_out   : 1; //< Empty Transmitter Holding Register.
		uint8_t empty_in    : 1; //< Empty Data Holding Registers.
		uint8_t fifo_err    : 1; //< Error in Received FIFO.
	};
	uint8_t raw;
};

//! The Modem Status Register.
typedef union uart_msr uart_msr_t;
union uart_msr
{
	struct __packed
	{
		uint8_t delta_out      : 1; //< Change in "Clear to Send".
		uint8_t delta_in       : 1; //< Change in "Data Set Ready".
		uint8_t delta_ring     : 1; //< Change in "Trailing Edge Ring".
		uint8_t delta_carrier  : 1; //< Change in "Carrier Detect".
		uint8_t ready_out      : 1; //< Clear to Send.
		uint8_t ready_in       : 1; //< Data Set Ready.
		uint8_t ring_active    : 1; //< Trailing Edge Ring Indicator.
		uint8_t carrier_active : 1; //< Carrier Detect.
	};
	uint8_t raw;
};

static inline uint16_t uart_divisor_get(size_t baud)
{
	return (uint16_t)(UART_FREQUENCY / baud);
}
