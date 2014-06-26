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

/*! \file arch/i586/hw/int/pic.h
 *  \date June 2014
 */

#pragma once

#include <sw/int/isr.h>
#include <sw/int/irq.h>
#include <stdint.h>
#include <stdbool.h>

#define PIC_CHIP_IRQ_MAX 8
#define PIC_REMAP_BASE 32

//! The interrupt base of each PIC chip.
typedef enum pic_chip PIC_CHIP;
enum pic_chip
{
	PIC_CHIP_MASTER = 0, //!< Referred to as PIC1.
	PIC_CHIP_SLAVE  = 8  //!< Referred to as PIC2.
};

//! The PIC is controlled through I/O ports.
/*! Each PIC chip has its own command and data port. */
typedef enum pic_port PIC_PORT;
enum pic_port
{
	PIC1_PORT_BASE = 0x20,
	PIC1_PORT_CMD  = 0x20,
	PIC1_PORT_DATA = 0x21,

	PIC2_PORT_BASE = 0xA0,
	PIC2_PORT_CMD  = 0xA0,
	PIC2_PORT_DATA = 0xA1
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

//! Special IRQs for the PIC chips.
typedef enum pic_irq PIC_IRQ;
enum pic_irq
{
	PIC_IRQ_CASCADE         = 2,
	PIC_IRQ_SPURIOUS_MASTER = 7,
	PIC_IRQ_SPURIOUS_SLAVE  = 15,

	PIC_IRQ_MAX = 16
};

//! PIC Initialization Control Word 1.
typedef union pic_icw1 pic_icw1_t;
union pic_icw1
{
	struct __attribute__ ((packed))
	{
		bool icw4          : 1; //!< ICW4 will be sent in this command.
		bool single        : 1; //!< This is the only PIC in the system.
		bool call_interval : 1; //!< Use PIC_CALL_INTERVAL.
		bool trigger       : 1; //!< Use PIC_TRIGGER.
		bool always_1      : 1;
		uint8_t            : 3;
	};
	struct __attribute__ ((packed))
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
	struct __attribute__ ((packed))
	{
		uint8_t vector8_15; //!< Vector bits 8-15.
	} mode_mcs8X;
	struct __attribute__ ((packed))
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
	struct __attribute__ ((packed))
	{
		uint8_t ir_slaves; //!< Each bit represents an IR-IN that has a slave.
	} master;
	struct __attribute__ ((packed))
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
	struct __attribute__ ((packed))
	{
		bool    sys_mode     : 1; //!< Use PIC_SYS_MODE.
		bool    auto_eoi     : 1;
		uint8_t buffer_mode  : 2; //!< Use PIC_BUFFER_MODE.
		bool    fully_nested : 1; //!< Enable special fully nested mode.
	};
	uint8_t raw;
};

void pic_init();
void pic_remap(isr_t pic1, isr_t pic2);
void pic_reset(PIC_CHIP pic);

void pic_irq_enable(irq_t irq);
void pic_irq_disable(irq_t irq);
void pic_irq_enable_all();
void pic_irq_disable_all();
