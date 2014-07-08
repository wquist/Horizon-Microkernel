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

#include "gdt.h"
#include <sw/instr/ldr.h>
#include <debug/log.h>
#include <debug/error.h>

static gdt_t kernel_table = {0};

//! Initialize a Global Descriptor Table by setting default entries and the GDTR.
void gdt_init(gdt_t* table)
{
	dassert(table);

	gdt_entry_set(table, GDT_SEGMENT_KCODE, 0x0, 0xFFFFFFFF, GDT_ENTRYTYPE_CODE, 0);
	gdt_entry_set(table, GDT_SEGMENT_KDATA, 0x0, 0xFFFFFFFF, GDT_ENTRYTYPE_DATA, 0);
	gdt_entry_set(table, GDT_SEGMENT_UCODE, 0x0, 0xFFFFFFFF, GDT_ENTRYTYPE_CODE, 3);
	gdt_entry_set(table, GDT_SEGMENT_UDATA, 0x0, 0xFFFFFFFF, GDT_ENTRYTYPE_DATA, 3);

	table->gdtr.base  = (uint32_t)&(table->entries[0]);
	table->gdtr.limit = GDT_SEGMENT_MAX * sizeof(gdt_entry_t) - 1;
}

//! Copy a GDT's entries to another table.
void gdt_clone(gdt_t* dest, const gdt_t* src)
{
	dassert(dest);
	dassert(src);

	for (size_t i = 0; i != GDT_SEGMENT_MAX; ++i)
		dest->entries[i].raw = src->entries[i].raw;

	dest->gdtr.base  = (uint32_t)&(dest->entries[0]);
	dest->gdtr.limit = GDT_SEGMENT_MAX * sizeof(gdt_entry_t) - 1;
}

//! Set a single segment entry in a GDT.
void gdt_entry_set(gdt_t* table, size_t i, uint32_t base, uint32_t limit, GDT_ENTRYTYPE type, size_t dpl)
{
	dassert(table);
	dassert(i < GDT_SEGMENT_MAX);
	dassert(dpl <= 3);

	gdt_entry_t *e = &(table->entries[i]);
	e->raw = 0ULL; //< Some unnamed fields must always be 0.

	e->limit_low   =  limit        & 0xFFFF;
	e->limit_high  = (limit >> 16) & 0xF;

	e->base_low	   =  base         & 0xFFFFFF;
	e->base_high   = (base  >> 24) & 0xFF;

	e->accessed    = (type == GDT_ENTRYTYPE_TSS); //< (alt) Differentiate between TSS and LDT.
	e->readwrite   = (type == GDT_ENTRYTYPE_DATA); //< Should self-modifying code be allowed?
	e->expand_down =  0;
	e->code        = (type == GDT_ENTRYTYPE_CODE || type == GDT_ENTRYTYPE_TSS);  //< (alt) 32-bit TSS.
	e->normal      = (type == GDT_ENTRYTYPE_CODE || type == GDT_ENTRYTYPE_DATA); //< 0 is TSS or LDT.
	e->privilege   =  dpl;
	e->present     =  1;

	e->available   =  0;
	e->width       = (type == GDT_ENTRYTYPE_CODE || type == GDT_ENTRYTYPE_DATA); //< 32-bit, or (alt) 0 for TSS.
	e->granularity = (type == GDT_ENTRYTYPE_CODE || type == GDT_ENTRYTYPE_DATA); //< Page granularity, only for code/data.

	dtrace("Set entry %i in GDT %p to 0x%08X%08X", i, table, (uint32_t)(e->raw), (uint32_t)(e->raw >> 32));
}

void gdt_load(const gdt_t* table)
{
	dassert(table);

	lgdt(&(table->gdtr));
	_ASM
	(
		"mov $0x10, %%ax;" //< Load the new kernel data segment to all segments. (except code)
		"mov %%ax,  %%ds;"
		"mov %%ax,  %%es;"
		"mov %%ax,  %%fs;"
		"mov %%ax,  %%gs;"
		"mov %%ax,  %%ss;"

		"ljmp $0x8, $.flush;" //< Jump to the new kernel code segment.
		".flush:;"
	::: "memory");
}

gdt_t* kernel_gdt()
{
	return &kernel_table;
}
