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

/*! \file arch/i586/sw/int/exception.h
 *  \date June 2014
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

//! The 32 exception interrupts.
/*! The ones not listed are reserved. */
typedef enum exception EXCEPTION;
enum exception
{
	EXCEPTION_MIN = 0,
    
	EXCEPTION_DE = 0,  //!< Divide-by-Zero Error
	EXCEPTION_DB,      //!< Debug Trap
	EXCEPTION_NMI,     //!< Non-Maskable Interrupt
	EXCEPTION_BP,      //!< Breakpoint Trap
	EXCEPTION_OF,      //!< Overflow Trap
	EXCEPTION_BR,      //!< Bound Range Exceeded
	EXCEPTION_UD,      //!< Invalid Opcode
	EXCEPTION_NM,      //!< Device Not Available
	EXCEPTION_DF,      //!< Double Fault
	EXCEPTION_TS = 10, //!< Invalid TSS
	EXCEPTION_NP,      //!< Segment Not Present
	EXCEPTION_SS,      //!< Stack-Segment Fault
	EXCEPTION_GP,      //!< General Protection Fault
	EXCEPTION_PF,      //!< Page Fault
	EXCEPTION_MF = 16, //!< x87 Floating-Poin Exception
	EXCEPTION_AC,      //!< Alignment Check
	EXCEPTION_MC,      //!< Machine Check
	EXCEPTION_XM,      //!< SIMD Floating-Point Exception
	EXCEPTION_DX = 30, //!< Security Exception
    
	EXCEPTION_MAX = 32
};

//! Some exceptions push an error code to the stack in this format.
typedef union exception_errcode exception_errcode_t;
union exception_errcode
{
	struct _Packed
	{
		bool     e     : 1;
		uint8_t  tbl   : 2;
		uint16_t index : 13;
	};
	uint16_t raw;
};

void exception_init();
