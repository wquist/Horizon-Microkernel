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

#include "exception.h"
#include <sw/instr/crx.h>
#include <sw/int/callback.h>
#include <sw/int/frame.h>
#include <sw/int/isr.h>
#include <sw/int/irq.h>
#include <debug/log.h>
#include <debug/error.h>
#include <stddef.h>

void exception_callback(isr_t isr, irq_t irq);
void exception_debug(isr_t isr, irq_t irq);

//! Reserve the exception ISRs and add callbacks.
void exception_init()
{
	for (size_t i = EXCEPTION_MIN; i != EXCEPTION_MAX; ++i)
	{
		isr_reserve(i);
		int_callback_set(i, false, exception_callback);
#if defined(_DEBUG)
		int_callback_set(i, true, exception_debug);
#endif
	}
}

// Properly handle an exception.
void exception_callback(isr_t isr, irq_t irq)
{
	//
}

// Print detailed information when an exception occurs.
void exception_debug(isr_t isr, irq_t irq)
{
	int_frame_t* frame = int_callback_frame_get();

	// Only some exceptions format this variable.
	exception_errcode_t sel = { .raw = frame->err_code };

	// The faulting table is set in the errcode, with these values.
	const char* table;
	switch (sel.tbl)
	{
		case 0:  table = "GDT"; break;
		case 2:  table = "LDT"; break;
		default: table = "IDT"; break;
	}

	const char* errout = NULL;
	switch (isr)
	{
		switch (isr)
		{
			case EXCEPTION_DE: errout = "Divide by zero error";          break;
			case EXCEPTION_OF: errout = "Overflow trap";                 break;
			case EXCEPTION_BR: errout = "Bound range exceeded";          break;
			case EXCEPTION_UD: errout = "Invalid opcode executed";       break;
			case EXCEPTION_NM: errout = "FPU used but not enabled";      break;
			case EXCEPTION_MF: errout = "x87 floating point exception";  break;
			case EXCEPTION_XM: errout = "SIMD floating point exception"; break;
			case EXCEPTION_DX: errout = "Security exception";            break;
			default: break;
		}
		if (errout)
		{
			dtrace("%s at 0x%08X.", errout, frame->EIP);
			break;
		}

		switch (isr)
		{
			case EXCEPTION_DF: errout = "Double fault";              break;
			case EXCEPTION_TS: errout = "Invalid TSS exception";     break;
			case EXCEPTION_NP: errout = "Segment not present fault"; break;
			case EXCEPTION_SS: errout = "Stack-segment fault";       break;
			case EXCEPTION_GP: errout = "General protection fault";  break;
			case EXCEPTION_AC: errout = "Alignment check exception"; break;
			default: break;
		}
		if (errout)
		{
			// All of the protection fault exceptions set the selector errcode.
			dtrace("%s (e:%i) in %s[%i] at 0x%08X.", errout, sel.e, table, sel.index, frame->EIP);
			break;
		}

		switch (isr)
		{
			case EXCEPTION_DB: errout = "Debug trap";      break;
			case EXCEPTION_BP: errout = "Breakpoint trap"; break;
			default: break;
		}
		if (errout)
		{
			dtrace("%s before 0x%08X.", errout, frame->EIP);
			break;
		}

		case EXCEPTION_NMI:
		{
			dtrace("Non-maskable interrupt; check SYSCTL ports for more info.");
			break;
		}

		case EXCEPTION_PF:
		{
			// Each bit represents cpu state when the bad page was accessed.
			bool p = !!(sel.raw & (1 << 0));
			bool w = !!(sel.raw & (1 << 1));
			bool u = !!(sel.raw & (1 << 2));
			bool r = !!(sel.raw & (1 << 3));
			bool i = !!(sel.raw & (1 << 4));

			dtrace("Page fault: 0x%08X -> 0x%08X (P:%i W:%i U:%i R:%i I:%i).", frame->EIP, cr2_read(), p,w,u,r,i);
			break;
		}

		default:
		{
			dpanic("Reserved/invalid exception occurred!");
			break;
		}
	}

#define REP_THRESHOLD 1
	static int rep_last  = -1; 
	static int rep_count = 0;

	if (rep_last == isr)
	{
		++rep_count;
		if (rep_count > REP_THRESHOLD)
			dpanic("Detected infinite exception loop; halting.");
	}
	else
	{
		rep_last  = isr;
		rep_count = 0;
	}

	// Print a register dump.
	int_frame_debug(frame);
}
