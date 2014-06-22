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

#include "idt.h"
#include <sw/gdt.h>
#include <debug/error.h>

static idt_t kernel_table = {0};

// Externally linked variables from /stubs/isr_wrapper.nasm.
extern const uintptr_t ___sisr;
extern const uintptr_t ___eisr;
const uintptr_t __sisr = (uintptr_t)&___sisr;
const uintptr_t __eisr = (uintptr_t)&___eisr;

//! Intialize an Interrupt Descriptor Table.
void idt_init(idt_t* table)
{
	dassert(table);

	for (size_t i = 0; i != IDT_ENTRY_MAX; ++i)
		idt_entry_set(table, i, GDT_SEGMENT_KCODE, IDT_ENTRYTYPE_INT32, 0);

	idt_entry_t* first = &(table->entries[0]);
	dtrace("Set all entries in IDT %p to 0x%08X%08X", table, (uint32_t)(first->raw), (uint32_t)(first->raw >> 32));

	table->idtr.base  = (uint32_t)first;
	table->idtr.limit = IDT_ENTRY_MAX * sizeof(idt_entry_t) - 1;
}

//! Set a single ISR entry in an IDT.
/*! In the kernel, all of the ISR stubs have the same number of instructions, 
 *  so their location can be determined using "base + offset * size".
 */
void idt_entry_set(idt_t* table, size_t i, uint16_t seg, IDT_ENTRYTYPE type, size_t dpl)
{
	dassert(table);
	dassert(i < IDT_ENTRY_MAX);
	dassert(seg < GDT_SEGMENT_MAX);
	dassert(dpl <= 3);

	idt_entry_t* e = &(table->entries[i]);
	e->raw = 0ULL;

	size_t   size = __eisr - __sisr;
	uint32_t base = __sisr + (size * i);

	e->base_low  =  base        & 0xFFFF;
	e->base_high = (base >> 16) & 0xFFFF;
	e->segment   =  GDT_SEGMENT_OFFSET(seg);

	e->gate_type =  type;
	e->storage   =  0;
	e->privilege =  dpl;
	e->present   =  1;
}

//! Set only the privilege level of an ISR.
void idt_entry_priv_set(idt_t* table, size_t i, size_t dpl)
{
	dassert(table);
	dassert(i < IDT_ENTRY_MAX);
	dassert(dpl <= 3);

	idt_entry_t* e = &(table->entries[i]);

	e->privilege = dpl;
}

idt_t* kernel_idt()
{
	return &kernel_table;
}
