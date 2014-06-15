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

/*! \file arch/i586/sw/instr/crx.h
 *  \date June 2014
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

//! Control register 0 flag bits.
typedef union cr0 cr0_t;
union cr0
{
	struct _Packed
	{
		bool PE  : 1;  //!< Protected Mode Enable.
		bool MP  : 1;  //!< Monitor Co-Processor. (Controls behavior of WAIT/FWAIT)
		bool EM  : 1;  //!< FPU Emulation. (FPU present if clear)
		bool TS  : 1;  //!< Task Switched. (Allows saving of x87 context after call)
		bool ET  : 1;  //!< Extension Type.
		bool NE  : 1;  //!< Numeric Error. (Enable internal x87 FPU error reports)
		uint16_t : 10;
		bool WP  : 1;  //!< Write Protect. (Clear allows CPU to write to read-only pages)
		bool     : 1;
		bool AM  : 1;  //!< Alignment Mask. (Enables alignment check with EFLAGS bit AC)
		uint16_t : 10;
		bool NW  : 1;  //!< Not-Write Through. (Set disables write-back caching)
		bool CD  : 1;  //!< Cache Disable. (Set disables the memory cache)
		bool PG  : 1;  //!< Paging.
	};
	uint32_t raw;
};

//! Control register 3 flag bits.
typedef union cr3 cr3_t;
union cr3
{
	struct _Packed
	{
		uint8_t       : 3;
		bool     PWT  : 1;  //!< Page Write Through
		bool     PCD  : 1;  //!< Page Cache Disable
		uint8_t       : 7;
		uint32_t PDBR : 20;
	};
	uint32_t raw;
};

//! Control register 4 flag bits.
typedef union cr4 cr4_t;
union cr4
{
	struct _Packed
	{
		bool VME      : 1; //!< Virtual 8086 Extensions. (Allow the VIF flag in V8086 mode)
		bool PVI      : 1; //!< Protected Virtual Interrupts. (Allow the VIF flag in protected mode)
		bool TSD      : 1; //!< Time Stamp Disable. (RDTSC can only be called from DPL0 when set)
		bool DE       : 1; //!< Debugging Extensions.
		bool PSE      : 1; //!< Page Size Extension.
		bool PAE      : 1; //!< Physical Address Extension.
		bool MCE      : 1; //!< Machine Check Exception.
		bool PGE      : 1; //!< Page Global Enabled. (Set allows global bit in PDEs and PTEs)
		bool PCE      : 1; //!< Perfomance Monitor Counter Enable (allows RDPMC at any DPL when set)
		bool OSFXSR   : 1; //!< Support for FXSAVE and FXSTOR
		bool OSXMEX   : 1; //!< Unmasked SIMD FP Exceptions.
		uint8_t       : 2;
		bool VMXE     : 1; //!< Virtual Machine Extensions. (Intel VT-x virtualization)
		bool SMXE     : 1; //!< Safe Mode Exceptions.
		bool          : 1;
		bool FSGSBASE : 1; //!< Enable WRFSBASE/WRGSBASE and RDFSBASE/RDGSBASE. (Write to %fs and %gs in any ring without MSRs)
		bool PCIDE    : 1; //!< PCID Enable (setting enables process-context identifiers)
		bool OSXSAVE  : 1; //!< XSAVE and Processor Extended States.
		bool          : 1;
		bool SMEP     : 1; //!< Supervisor Execution Protection. (when set fault occurs when low DPL executes high DPL code)
		bool SMAP     : 1; //!< Supervisor Access Protection. (when set fault occurs when low DPL accesses high DPL data)
		uint16_t      : 10;
	};
	uint32_t raw;
};

// Defines crx_read() and crx_write().
#define crx_rw_pair_IMP(x) \
static inline uint32_t cr##x##_read() { uint32_t ret; _ASM ("movl %%cr"#x", %0" : "=a" (ret)); return ret; } \
static inline void cr##x##_write(uint32_t val) { _ASM ("movl %0, %%cr"#x :: "r" (val) : "memory"); }

crx_rw_pair_IMP(0); //!< Processor flags like pmode, v8086, caching, and paging.
crx_rw_pair_IMP(2); //!< Source address after a page fault.
crx_rw_pair_IMP(3); //!< Pointer to the current page directory.
crx_rw_pair_IMP(4); //!< More flags like v8086, breakpoints, PAE, PSE, and debugging.

#undef crx_rw_pair_IMP
