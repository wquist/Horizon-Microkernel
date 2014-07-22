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
#include <spec/pic8259.h>
#include <spec/pic8259/x86.h>

void pic_eoi(isr_t isr, irq_t irq);

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

	// Edge triggered, will send ICW4.
	pic_icw1_t icw1 = {0};
	icw1.icw4          = true;
	icw1.single        = false;
	icw1.call_interval = PIC_CALL_INTERVAL_8;
	icw1.trigger       = PIC_TRIGGER_EDGE;
	icw1.always_1      = 1;

	// Send the same data to both PICs.
	pic_icw_write(icw1.raw, icw1.raw, true);

	// ICW2 contains the remap offsets.
	pic_icw2_t icw2_m = { .raw = pic1 };
	pic_icw2_t icw2_s = { .raw = pic2 };

	// ICW2 is different for the two PICs.
	pic_icw_write(icw2_m.raw, icw2_s.raw, false);

	// Setup master/slave mode with ICW3.
	pic_icw3_t icw3_m = {0};
	icw3_m.master.ir_slaves = (1 << PIC_IRQ_CASCADE); //< Slave connected to IRQ2.
	pic_icw3_t icw3_s = {0};
	icw3_s.slave.slave_id = 2; //< This slave is second in line.

	// ICW3 is also different.
	pic_icw_write(icw3_m.raw, icw3_s.raw, false);

	// Setup other special modes (all disabled).
	pic_icw4_t icw4 = {0};
	icw4.sys_mode     = PIC_SYS_MODE_808X;
	icw4.auto_eoi     = false;
	icw4.buffer_mode  = PIC_BUFFER_MODE_NONE;
	icw4.fully_nested = false;

	pic_icw_write(icw4.raw, icw4.raw, false);
}

// Reset the appropriate PICs after an interrupt.
void pic_eoi(isr_t isr, irq_t irq)
{
	bool needs_eoi = true;
	if (irq == PIC_IRQ_SPURIOUS_MASTER || irq == PIC_IRQ_SPURIOUS_SLAVE)
	{
		uint16_t irqs = pic_isr_read();
		// If the interrupt was spurious, the corresponding bit will be clear.
		needs_eoi = (irqs & (1 << irq)); //< No EOI if the IRQ is spurious.
	}

	if (needs_eoi && irq >= PIC_CHIP_SLAVE)
		pic_reset(PIC_CHIP_SLAVE);
	// Even for the slave spurious IRQ, master needs an EOI because of cascade.
	if (needs_eoi || irq >= PIC_CHIP_SLAVE)
		pic_reset(PIC_CHIP_MASTER);
}
