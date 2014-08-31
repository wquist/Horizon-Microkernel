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

#include "syscall.h"
#include <sw/int/isr.h>
#include <sw/int/idt.h>
#include <debug/log.h>
#include <debug/error.h>

extern const syscall_t syscall_table[];
static size_t table_size = 0;

static void dispatch_call(isr_t isr, irq_t irq);

//! Initialize the syscall interrupt handler.
void syscall_init()
{
	isr_reserve(SYSCALL_ISR);
	// Allow a syscall int to be fired from usermode.
	idt_entry_priv_set(kernel_idt(), SYSCALL_ISR, 3);
	int_callback_set(SYSCALL_ISR, false, dispatch_call);

	while (syscall_table[table_size].function.raw)
		++table_size;

	dtrace("Added ISR %#X callback for %i system calls.", SYSCALL_ISR, table_size);
}

// Call the requested syscall from syscall_table.
void dispatch_call(isr_t isr, irq_t irq)
{
	int_frame_t* frame = int_callback_frame_get();

	size_t index = frame->EAX;
	if(index >= table_size)
	{
		dtrace("Warning: invalid system call %i invoked.", index);
		return;
	}

	const syscall_t* call = &(syscall_table[index]);
	switch (call->arg_count)
	{
		case 0:
			call->function.arg0();
			break;
		case 1:
			call->function.arg1(frame->EBX);
			break;
		case 2:
			call->function.arg2(frame->EBX, frame->ECX);
			break;
		case 3:
			call->function.arg3(frame->EBX, frame->ECX, frame->EDX);
			break;
		default: dpanic("Syscall has too many parameters!");
	}
}
