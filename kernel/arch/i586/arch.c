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

#include "arch.h"
#include <sw/gdt.h>
#include <sw/int/idt.h>
#include <hw/int/pic.h>
#include <hw/pit.h>
#include <debug/log.h>
#include <horizon/io.h>
#include <horizon/errno.h>
#include <limits.h>
#include <stdbool.h>

// The actual value of a linker variable is its address.
#define LINKVAR(symbol, addr)  \
extern const uintptr_t symbol; \
const uintptr_t addr = (uintptr_t)&symbol;

LINKVAR(_KERNEL_PHYS_ADDR, KERNEL_PHYS_ADDR);
LINKVAR(_KERNEL_VIRT_ADDR, KERNEL_VIRT_ADDR);
LINKVAR(_KERNEL_VIRT_BASE, KERNEL_VIRT_BASE);
LINKVAR(___skernel, __skernel);
LINKVAR(___ekernel, __ekernel);

#undef LINKVAR

void arch_init()
{
	gdt_init(kernel_gdt());
	gdt_load(kernel_gdt());
	dtrace("Installed main kernel GDT. (null|kcode|kdata|ucode|udata)");

	idt_init(kernel_idt());
	idt_load(kernel_idt());
	dtrace("Installed main kernel IDT. (interrupts are INT32 ring 0)");
}

void arch_timer_init(size_t freq, int_callback_t handle)
{
	int_callback_set(irq_to_isr(PIT_IRQ), false, handle);
	pic_irq_enable(PIT_IRQ);
	pit_timer_set(PIT_TIMER0, freq, PIT_OUTMODE_SQRWAVE);
}

int arch_sysio(size_t action, size_t arg, uintptr_t data)
{
	switch (action)
	{
		case IO_INB:
		{
			// Make sure the values are within valid bounds.
			if (arg > USHORT_MAX)
				return EPARAM;

			return inb((uint16_t)arg);
		}
		case IO_INW:
		{
			if (arg > USHORT_MAX)
				return EPARAM;

			return inw((uint16_t)arg);
		}
		case IO_OUTB:
		{
			if (arg > USHORT_MAX)
				return EPARAM;
			if (data > UCHAR_MAX)
				return EPARAM;

			outb((uint16_t)arg, (uint8_t)data);
			return ENONE;
		}
		case IO_OUTW:
		{
			if (arg > USHORT_MAX)
				return EPARAM;
			if (data > USHORT_MAX)
				return EPARAM;

			outw((uint16_t)arg, (uint16_t)data);
			return ENONE;
		}
		default: return EPARAM;
	}
}
