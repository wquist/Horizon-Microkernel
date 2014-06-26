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

#include "frame.h"
#include <debug/log.h>
#include <stdio.h>

//! Dump the contents of the register frame.
void int_frame_debug(int_frame_t* frame)
{
	dtrace("Register Dump:");
	printf("\tEBP:0x%08X EIP:0x%08X ESP:0x%08X\n", frame->EBP, frame->EIP, frame->ESP);
	printf("\tEAX:0x%08X EBX:0x%08X\n", frame->EAX, frame->EBX);
	printf("\tECX:0x%08X EDX:0x%08X\n", frame->ECX, frame->EDX);
	printf("\tEFLAGS:0x%08X\n", frame->EFLAGS);
	dtrace("Segment Dump:");
	printf("\tCS:0x%08X, DS:0x%08X, SS:0x%08X\n", frame->CS, frame->DS, frame->SS);
}
