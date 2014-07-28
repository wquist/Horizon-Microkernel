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

/*! \file arch/i586/sw/int/idt.h
 *  \date June 2014
 */

#pragma once

#include <sw/instr/ldr.h>
#include <stddef.h>
#include <stdint.h>

#define IDT_ENTRY_MAX 256

//! The type of interrupt to be triggered.
/*! Each type has a different behavior when an
 *  interrupt is triggered (16-bit should be used
 *  int real mode, 32-bit in protected mode):
 *  - TASK: A task switch will be performed when
 *          the interrupt is triggered. Only useful
 *          for hardware task switching.
 *  - INT:  An ISR is called, pushing the current
 *          machine state to the stack.
 *  - TRAP: Works the same as an INT, except interrupts
 *          are not disabled while in the ISR.
 */
typedef enum idt_entrytype IDT_ENTRYTYPE;
enum idt_entrytype
{
	IDT_ENTRYTYPE_TASK32 = 0x5,
	IDT_ENTRYTYPE_INT16  = 0x6,
	IDT_ENTRYTYPE_TRAP16 = 0x7,
	IDT_ENTRYTYPE_INT32  = 0xE,
	IDT_ENTRYTYPE_TRAP32 = 0xF
};

//! An IDT description. Identical to the GDTR.
typedef struct idt_desc idt_desc_t;
struct __packed idt_desc
{
	uint16_t limit; //!< The size.
	uint32_t base;  //!< The address.
};

//! A single IDT entry.
typedef union idt_entry idt_entry_t;
union idt_entry
{
	struct __packed
	{
		uint16_t base_low  : 16;
		uint16_t segment   : 16; //!< The GDT segment the ISR should be called from.
		uint8_t            : 8;  //!< Always 0.

		uint8_t  gate_type : 4;
		uint8_t  storage   : 1;  //!< Should be 0 for interrupt gates.
		uint8_t  privilege : 2;
		uint8_t  present   : 1;

		uint16_t base_high : 16;
	};
	uint64_t raw;
};

//! A complete IDT. Holds all entries and an IDTR.
typedef struct idt idt_t;
struct idt
{
	idt_entry_t entries[IDT_ENTRY_MAX];
	idt_desc_t  idtr;
};

static inline void idt_load(const idt_t* table) { lidt(&(table->idtr)); }

void idt_init(idt_t* table);
void idt_entry_set(idt_t* table, size_t i, uint16_t seg, IDT_ENTRYTYPE type, size_t dpl);
void idt_entry_priv_set(idt_t* table, size_t i, size_t dpl);

idt_t* kernel_idt();
