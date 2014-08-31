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

/*! \file spec/pit8253.h
 *  \date July 2014
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#define PIT_BASE_FREQ 1193180

//! A PIT has three individual timers.
typedef enum pit_timer PIT_TIMER;
enum pit_timer
{
	PIT_TIMER0 = 0, //!< The main clock.
	PIT_TIMER1,     //!< Obsolete (RAM refresh).
	PIT_TIMER2,     //!< Connected to PC speaker.
	PIT_READBACK    //!< Not available on 8253.
};

//! Access flags for PIT commands.
typedef enum pit_access PIT_ACCESS;
enum pit_access
{
	PIT_ACCESS_LATCH = 0, //!< Prevents count from updating.
	PIT_ACCESS_RW_LOW,    //!< Only access low byte.
	PIT_ACCESS_RW_HIGH,   //!< Only access high byte.
	PIT_ACCESS_RW_BOTH    //!< Access both bytes.
};

//! Modes that determine when and how interrupts are fired.
typedef enum pit_outmode PIT_OUTMODE;
enum pit_outmode
{
	//! Wait for reload, trigger on falling edge. Stays high.
	PIT_OUTMODE_TERMCNT = 0,
	//! Wait for rising edge of gate input to start. Stays high.
	PIT_OUTMODE_ONESHOT,
	//! Frequency divider. Resets to reload value on falling edge of 0.
	PIT_OUTMODE_RATEGEN,
	//! Works like RATEGEN, but flip-flops for square-wave vs pulse.
	PIT_OUTMODE_SQRWAVE,
	//! Delay, pulses on 0. Waits for reload, triggers on falling edge.
	PIT_OUTMODE_SWTRIG,
	//! Similar to SWTRIG, only for channel 2. Waits for gate rising edge.
	PIT_OUTMODE_HWTRIG
};

//! Selectable counting mode.
typedef enum pit_cntmode PIT_CNTMODE;
enum pit_cntmode
{
	PIT_CNTMODE_BIN = 0, //!< Binary.
	PIT_CNTMODE_BCD,     //!< Decimal.
};

//! The PIT command format.
typedef union pit_command pit_command_t;
union pit_command
{
	//! A setup command.
	struct __packed
	{
		uint8_t count_mode  : 1; //!< PIT_CNTMODE.
		uint8_t output_mode : 3; //!< PIT_OUTMODE.
		uint8_t access_mode : 2; //!< PIT_ACCESS.
		uint8_t channel     : 2; //!< TIMERX the command is targeting.
	};
	//! A status command.
	struct __packed
	{
		uint8_t             : 1; //!< Should be 0.
		uint8_t readback_t0 : 1; //!< Get TIMER0 status.
		uint8_t readback_t1 : 1; //!< TIMER1.
		uint8_t readback_t2 : 1; //!< TIMER2.
		uint8_t no_status   : 1; //!< Do not retrieve the status from data reg.
		uint8_t no_count    : 1; //!< Do not read the latch count.
		uint8_t always_1    : 2; //!< Each bit should be set to 1.
	};
	uint8_t raw;
};

//! The returned status after a readback.
typedef union pit_status pit_status_t;
union pit_status
{
	struct __packed
	{
		uint8_t count_mode   : 1;
		uint8_t output_mode  : 3;
		uint8_t access_mode  : 2;
		//! Set when a value is reloaded and not the current count.
		uint8_t loaded       : 1;
		//! Reflects the state of the specified timer output pin.
		uint8_t output_state : 1;
	};
	uint8_t raw;
};

//! The PIT control format.
typedef union pit_control pit_control_t;
union pit_control
{
	// FIXME: Properly document these bits.
	struct __packed
	{
		uint8_t gate_ch2      : 1;
		uint8_t speaker       : 1;
		uint8_t check_parity  : 1;
		uint8_t check_io      : 1;
		uint8_t out_ch1       : 1;
		uint8_t out_ch2       : 1;
		uint8_t parity_result : 1;
		uint8_t io_result     : 1;
	};
	uint8_t raw;
};
