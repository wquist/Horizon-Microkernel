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

/*! \file spec/pic8259/io.h
 *  \date July 2014
 */

#include "../pic8259.h"

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

void pic_icw_write(uint8_t mval, uint8_t sval, bool cmd);
uint16_t pic_isr_read();
void pic_reset(PIC_CHIP pic);

void pic_irq_enable(uint8_t irq);
void pic_irq_disable(uint8_t irq);
void pic_irq_enable_all();
void pic_irq_disable_all();
