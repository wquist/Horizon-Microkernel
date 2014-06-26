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

/*! \file arch/i586/sw/int/frame.h
 *  \date June 2014
 */

#pragma once

#include <stdint.h>

//! The registers that are pushed to the stack during an interrupt.
/*! \note The order is important, this struct is cast from the stack
 *  set up by the interrupt assembly wrapper.
 */
typedef struct int_frame int_frame_t;
struct int_frame
{
	uint32_t DS;                 //!< Data segment.
	uint32_t EDI, ESI;           //!< String registers.
	uint32_t EBP, ESP;           //!< Base and stack pointers.
	uint32_t EBX, EDX, ECX, EAX; //!< Named GP registers.
	uint32_t int_num;            //!< Source ISR number.

	uint32_t err_code;           //!< Error code (if applicable).
	uint32_t EIP, CS;            //!< Instruction pointer and code segment.
	uint32_t EFLAGS;             //!< EFLAGS state before interrupt.
	uint32_t user_esp, SS;       //!< User stack pointer and stack segment.
};

void int_frame_debug(int_frame_t* frame);
