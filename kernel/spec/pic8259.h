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

/*! \file spec/pic8259.h
 *  \date July 2014
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#define PIC_CHIP_IRQ_MAX 8

//! The interrupt base of each PIC chip.
typedef enum pic_chip PIC_CHIP;
enum pic_chip
{
	PIC_CHIP_MASTER = 0, //!< Referred to as PIC1.
	PIC_CHIP_SLAVE  = 8  //!< Referred to as PIC2.
};

//! Special IRQs for the PIC chips.
typedef enum pic_irq PIC_IRQ;
enum pic_irq
{
	PIC_IRQ_CASCADE         = 2,
	PIC_IRQ_SPURIOUS_MASTER = 7,
	PIC_IRQ_SPURIOUS_SLAVE  = 15,

	PIC_IRQ_MAX = 16
};

//! The different commands to send to the PITX_CMD port.
typedef enum pic_cmd PIC_CMD;
enum pic_cmd
{
	PIC_CMD_EOI      = 0x20,
	PIC_CMD_READ_IRR = 0x0A,
	PIC_CMD_READ_ISR = 0x0B
};

//! The two call address intervals.
/*! \todo Determine use/function. */
typedef enum pic_call_interval PIC_CALL_INTERVAL;
enum pic_call_interval
{
	PIC_CALL_INTERVAL_8 = 0,
	PIC_CALL_INTERVAL_4 = 1
};

//! PIC IRQs can be edge or level triggered.
typedef enum pic_trigger PIC_TRIGGER;
enum pic_trigger
{
	PIC_TRIGGER_EDGE  = 0,
	PIC_TRIGGER_LEVEL = 1
};

//! The PIC has special functions for MCS-80/85 and 8086/8088 systems.
/*! Only the 8086 functions are used here. */
typedef enum pic_sys_mode PIC_SYS_MODE;
enum pic_sys_mode
{
	PIC_SYS_MODE_MCS8X = 0,
	PIC_SYS_MODE_808X  = 1
};

//! Enable buffering on this PIC.
/*! Choose MASTER/SLAVE depending on what the PIC chip is set to be. */
typedef enum pic_buffer_mode PIC_BUFFER_MODE;
enum pic_buffer_mode
{
	PIC_BUFFER_MODE_NONE   = 0,
	PIC_BUFFER_MODE_SLAVE  = 2,
	PIC_BUFFER_MODE_MASTER = 3
};

//! PIC Initialization Control Word 1.
typedef union pic_icw1 pic_icw1_t;
union pic_icw1
{
	struct __packed
	{
		bool icw4          : 1; //!< ICW4 will be sent in this command.
		bool single        : 1; //!< This is the only PIC in the system.
		bool call_interval : 1; //!< Use PIC_CALL_INTERVAL.
		bool trigger       : 1; //!< Use PIC_TRIGGER.
		bool always_1      : 1;
		uint8_t            : 3;
	};
	struct __packed
	{
		uint8_t           : 5;
		uint8_t vector3_5 : 3; //!< Vector bits 3-5.
	} mode_msc8X;
	uint8_t raw;
};

//! PIC Initialization Control Word 2.
typedef union pic_icw2 pic_icw2_t;
union pic_icw2
{
	struct __packed
	{
		uint8_t vector8_15; //!< Vector bits 8-15.
	} mode_mcs8X;
	struct __packed
	{
		uint8_t  : 3;
		uint8_t vector3_7 : 5; //!< Vector bits 3-7.
	} mode_808X;
	uint8_t raw;
};

//! PIC Initialization Control Word 3.
typedef union pic_icw3 pic_icw3_t;
union pic_icw3
{
	struct __packed
	{
		uint8_t ir_slaves; //!< Each bit represents an IR-IN that has a slave.
	} master;
	struct __packed
	{
		uint8_t slave_id : 3;
		uint8_t          : 5;
	} slave;
	uint8_t raw;
};

//! PIC Initialization Control Word 4.
typedef union pic_icw4 pic_icw4_t;
union pic_icw4
{
	struct __packed
	{
		bool    sys_mode     : 1; //!< Use PIC_SYS_MODE.
		bool    auto_eoi     : 1;
		uint8_t buffer_mode  : 2; //!< Use PIC_BUFFER_MODE.
		bool    fully_nested : 1; //!< Enable special fully nested mode.
	};
	uint8_t raw;
};
