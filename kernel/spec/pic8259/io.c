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
#include <debug/error.h>

//! Send an initialization control word.
/*! 'cmd' should only be true for ICW1. */
void pic_icw_write(uint8_t mval, uint8_t sval, bool cmd)
{
	if (cmd)
	{
		outb(PIC1_PORT_CMD, mval);
		outb(PIC2_PORT_CMD, sval);
	}
	else
	{
		outb(PIC1_PORT_DATA, mval);
		outb(PIC2_PORT_DATA, sval);
	}
}

//! Get the In-Service Register value.
uint16_t pic_isr_read()
{
	// Send the In-Service Register (ISR) read request.
	outb(PIC1_PORT_CMD, PIC_CMD_READ_ISR);
	outb(PIC2_PORT_CMD, PIC_CMD_READ_ISR);

	// Read the actual ISR.
	uint16_t irqs = inb(PIC1_PORT_CMD) | (inb(PIC2_PORT_CMD) << 8);
	return irqs;
}

//! Send an EOI to the specified PIC.
void pic_reset(PIC_CHIP chip)
{
	if (chip == PIC_CHIP_MASTER)
		outb(PIC1_PORT_CMD, PIC_CMD_EOI);
	else
		outb(PIC2_PORT_CMD, PIC_CMD_EOI);
}

//! Allow the specified IRQ to be raised.
void pic_irq_enable(uint8_t irq)
{
	dassert(irq >= 0 && irq < PIC_IRQ_MAX);

	// Determine whether this IRQ belongs to master or slave.
	uint16_t port = (irq < PIC_CHIP_SLAVE) ? PIC1_PORT_DATA : PIC2_PORT_DATA;
	if (port == PIC2_PORT_DATA) irq -= PIC_CHIP_IRQ_MAX;

	// An interrupt is enabled when the corresponding bit is clear in PORT_DATA.
	uint8_t mask = inb(port) & ~(1 << irq);
	outb(port, mask);
}

//! Prevent the specified IRQ from being triggered.
void pic_irq_disable(uint8_t irq)
{
	dassert(irq >= 0 && irq < PIC_IRQ_MAX);

	uint16_t port = (irq < PIC_CHIP_SLAVE) ? PIC1_PORT_DATA : PIC2_PORT_DATA;
	if (port == PIC2_PORT_DATA) irq -= PIC_CHIP_IRQ_MAX;

	// The interrupt is disabled when the bit is set in PORT_DATA.
	uint8_t mask = inb(port) | (1 << irq);
	outb(port, mask);
}

//! Allow all interrupts from the PICs.
void pic_irq_enable_all()
{
	// Clear all bits in both PORT_DATA's.
	outb(PIC1_PORT_DATA, 0x00);
	outb(PIC2_PORT_DATA, 0x00);
}

//! Supress all interrupts from the PICs.
void pic_irq_disable_all()
{
	// Set all bits in both PORT_DATA's.
	outb(PIC1_PORT_DATA, 0xFF);
	outb(PIC2_PORT_DATA, 0xFF);
}
