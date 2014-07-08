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

/*! \file arch/i586/sw/task.h
 *  \date July 2014
 */

#pragma once

#include <sw/int/frame.h>
#include <stdint.h>

//! Arch-specific task info.
/*! Will always have the entry and stack fields. */
typedef struct task_info task_info_t;
struct task_info
{
	uintptr_t entry;
	uintptr_t stack;

	int_frame_t frame;
};

//! The TSS format. Only one per CPU.
typedef struct tss tss_t;
struct _Packed tss
{
	uint32_t prev; //!< Unused for SW task switching.
	
	uint32_t ESP0, SS0;
	//! ESP/SS 1 and 2 are not used as well.
	uint32_t ESP1, SS1;
	uint32_t ESP2, SS2;

	uint32_t CR3;
	uint32_t EIP;
	uint32_t EFLAGS;

	uint32_t EAX, ECX, EDX, EBX;
	uint32_t ESP, EBP, ESI, EDI;
	uint32_t ES, CS, SS, DS, FS, GS;

	//! These are also unused.
	uint32_t ldt;
	uint32_t trap;
	uint32_t iomap_base;
};

void task_init(task_info_t* info);

void task_start(task_info_t* info);
void task_switch(task_info_t* curr, task_info_t* next, int_frame_t* frame);

void tss_init(tss_t* task, uintptr_t kstack);
void tss_load(const tss_t* task);
