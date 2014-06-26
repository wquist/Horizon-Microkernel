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

#include "pic.h"
#include <sw/instr/io.h>
#include <sw/int/frame.h>
#include <sw/int/callback.h>
#include <debug/log.h>
#include <debug/error.h>

void pic_eoi(isr_t isr, irq_t irq, int_frame_t* frame);

//! Remap the PIC to make room for the exception ISRs and set up EOI handles.
void pic_init()
{
	dtrace("Initializing PIC; remapping IRQs and setting EOI callbacks.");

	pic_remap(PIC_CHIP_MASTER+PIC_REMAP_BASE, PIC_CHIP_SLAVE+PIC_REMAP_BASE);
	pic_irq_disable_all();

	for (irq_t i = 0; i != PIC_IRQ_MAX; ++i)
	{
		isr_t isr = PIC_REMAP_BASE + i;

		isr_reserve(isr);
		irq_set_isr(i, isr);
		int_callback_set(isr, true, pic_eoi);
	}
}

//! Map the PIC's IRQs to new ISR numbers.
void pic_remap(isr_t pic1, isr_t pic2)
{
	// The remap uses up to ISR picX + 8.
	dassert(pic1 <= ISR_MAX - PIC_CHIP_IRQ_MAX);
	dassert(pic2 <= ISR_MAX - PIC_CHIP_IRQ_MAX);

	// Save the previous IRQ mask info.
	uint8_t prev1 = inb(PIC1_PORT_DATA);
	uint8_t prev2 = inb(PIC2_PORT_DATA);

	// Edge triggered, will send ICW4.
	pic_icw1_t icw1 = {0};
	icw1.icw4          = true;
	icw1.single        = false;
	icw1.call_interval = PIC_CALL_INTERVAL_8;
	icw1.trigger       = PIC_TRIGGER_EDGE;
	icw1.always_1      = 1;

	// Send the same command to both PICs.
	outb(PIC1_PORT_CMD, icw1.raw);
	outb(PIC2_PORT_CMD, icw1.raw);

	// ICW2 contains the remap offsets.
	pic_icw2_t icw2_m = { .raw = pic1 };
	pic_icw2_t icw2_s = { .raw = pic2 };

	outb(PIC1_PORT_DATA, icw2_m.raw);
	outb(PIC2_PORT_DATA, icw2_s.raw);

	// Setup master/slave mode with ICW3.
	pic_icw3_t icw3_m = {0};
	icw3_m.master.ir_slaves = (1 << PIC_IRQ_CASCADE); //< Slave connected to IRQ2.
	pic_icw3_t icw3_s = {0};
	icw3_s.slave.slave_id = 2; //< This slave is second in line.

	outb(PIC1_PORT_DATA, icw3_m.raw);
	outb(PIC2_PORT_DATA, icw3_s.raw);

	// Setup other special modes (all disabled).
	pic_icw4_t icw4 = {0};
	icw4.sys_mode     = PIC_SYS_MODE_808X;
	icw4.auto_eoi     = false;
	icw4.buffer_mode  = PIC_BUFFER_MODE_NONE;
	icw4.fully_nested = false;

	// Send the same data to both chips.
	outb(PIC1_PORT_DATA, icw4.raw);
	outb(PIC2_PORT_DATA, icw4.raw);

	// Restore the previous IRQ mask info.
	outb(PIC1_PORT_DATA, prev1);
	outb(PIC2_PORT_DATA, prev2);
}

//! Allow the specified IRQ to be raised.
void pic_irq_enable(irq_t irq)
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
void pic_irq_disable(irq_t irq)
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

// Reset the appropriate PICs after an interrupt.
void pic_eoi(isr_t isr, irq_t irq, int_frame_t* frame)
{
	bool needs_eoi = true;
	if (irq == PIC_IRQ_SPURIOUS_MASTER || irq == PIC_IRQ_SPURIOUS_SLAVE)
	{
		// Send the In-Service Register (ISR) read request.
		outb(PIC1_PORT_CMD, PIC_CMD_READ_ISR);
		outb(PIC2_PORT_CMD, PIC_CMD_READ_ISR);

		// Read the actual ISR.
		uint16_t irqs = inb(PIC1_PORT_CMD) | (inb(PIC2_PORT_CMD) << 8);
		// If the interrupt was spurious, the corresponding bit will be clear.
		needs_eoi = (irqs & (1 << irq)); //< No EOI if the IRQ is spurious.
	}

	if (needs_eoi && irq >= PIC_CHIP_SLAVE)
		outb(PIC2_PORT_CMD, PIC_CMD_EOI);
	// Even for the slave spurious IRQ, master needs an EOI because of cascade.
	if (needs_eoi || irq >= PIC_CHIP_SLAVE)
		outb(PIC1_PORT_CMD, PIC_CMD_EOI);
}
