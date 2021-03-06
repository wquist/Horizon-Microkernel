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

/*! \file arch/i586/sw/gdt.h
 *  \date June 2014
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

//! The segments of a GDT.
/*! Each GDT is fixed and will always have these segments. */
typedef enum gdt_segment GDT_SEGMENT;
enum gdt_segment
{
	GDT_SEGMENT_NULL = 0,
	GDT_SEGMENT_KCODE,
	GDT_SEGMENT_KDATA,
	GDT_SEGMENT_UCODE,
	GDT_SEGMENT_UDATA,
	GDT_SEGMENT_TSS,

	GDT_SEGMENT_MAX
};

//! Used for streamlining setting of GDT entries.
typedef enum gdt_entrytype GDT_ENTRYTYPE;
enum gdt_entrytype
{
	GDT_ENTRYTYPE_CODE = 1,
	GDT_ENTRYTYPE_DATA,
	GDT_ENTRYTYPE_TSS,
	GDT_ENTRYTYPE_LDT //! \warning Not yet supported.
};

//! The description portion of a GDT.
/*! The limit should be the actual size - 1. */
typedef struct gdt_desc gdt_desc_t;
struct __packed gdt_desc
{
	uint16_t limit; //!< The size.
	uint32_t base;  //!< The address.
};

//! A single GDT entry.
/*! A GDT entry consists of a 64-bit bitfield of flags.
 *  The flags carry different meaning depending on whether the
 *  entry is a GDT segment entry, a TSS entry, or an LDT entry.
 */
typedef union gdt_entry gdt_entry_t;
union gdt_entry
{
	struct __packed
	{
		uint64_t limit_low   : 16;
		uint64_t base_low    : 24;

		uint64_t accessed    :  1; //!< Can also differentiate TSS/LDT (1 for TSS).
		uint64_t readwrite   :  1;
		uint64_t expand_down :  1;
		uint64_t code        :  1; //!< Can also be width of TSS (1 for 32-bit).
		uint64_t normal      :  1; //!< Always 1 for segments, 0 for TSS/LDT.
		uint64_t privilege   :  2;
		uint64_t present     :  1;

		uint64_t limit_high  :  4;

		uint64_t available   :  1; //!< \todo Should this always be 0 for segments?
		uint64_t             :  1; //!< Always 0.
		uint64_t width       :  1; //!< 1 for 32-bits (leave 0 when TSS).
		uint64_t granularity :  1; //!< 1 for page granularity.

		uint64_t base_high   :  8;
	};
	uint64_t raw;
};

//! A complete GDT. Holds all entries and a GDTR.
typedef struct gdt gdt_t;
struct gdt
{
	gdt_entry_t entries[GDT_SEGMENT_MAX];
	gdt_desc_t  gdtr;
};

//! Get the byte offset of a GDT_SEGMENT.
#define GDT_SEGMENT_OFFSET(x) ((size_t)(x) * 8)

void gdt_init(gdt_t* table);
void gdt_clone(gdt_t* dest, const gdt_t* src);
void gdt_entry_set(gdt_t* table, size_t i, uint32_t base, uint32_t limit, GDT_ENTRYTYPE type, size_t dpl);

void gdt_load(const gdt_t* table);

gdt_t* kernel_gdt();
