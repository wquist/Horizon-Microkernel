; Horizon Microkernel - Hobby Operating System
; Copyright (C) 2014 Wyatt Lindquist
; 
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
; 
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.

; isr_wrapper.nasm
; - Assembly stubs defined for all 256 interrupts.

; ===================
; Globals and Externs
; ===================

GLOBAL ___sisr, ___eisr
EXTERN int_handler_common

; ======
; Macros
; ======

%MACRO ISR_STUB_NOERR 1
isr%1: push %1
       jmp isr_noerr_entry
%ENDMACRO

%MACRO ISR_STUB_ERR 1
isr%1: push %1
       jmp isr_entry
%ENDMACRO

; =========
; ISR Stubs
; =========
SECTION .isrs
BITS 32

___sisr:
ISR_STUB_NOERR 0 ; Define the first ISR seperately to get the size
___eisr:

%ASSIGN i 1
%REP 7
	ISR_STUB_NOERR i ; ISR 1-7
%ASSIGN i i+1
%ENDREP

ISR_STUB_ERR   8
ISR_STUB_NOERR 9

%ASSIGN i 10
%REP 5
	ISR_STUB_ERR i ; ISR 10-14
%ASSIGN i i+1
%ENDREP

%ASSIGN i 15
%REP 241
	ISR_STUB_NOERR i ; ISR 15-255
%ASSIGN i i+1
%ENDREP

; ===========
; ISR Wrapper
; ===========

isr_noerr_entry: sub   esp, 4             ; Adjust the stack to insert the dummy error code
                 push  eax
                 mov   eax, [esp + 8]     ; Has to be inserted before the ISR number
                 mov   [esp + 4], eax
                 pop   eax
                 mov   dword [esp + 4], 0

isr_entry:       pusha
                 mov   eax, ds
                 push  eax

                 mov   ax, 0x10 ; Load the kernel data segment
                 mov   ds, ax
                 mov   es, ax
                 mov   fs, ax
                 mov   gs, ax

                ;call  int_handler_common

                 pop   eax    ; Restore the previous user data segment
                 mov   ds, ax
                 mov   es, ax
                 mov   fs, ax
                 mov   gs, ax

                 popa
                 add   esp, 8
                 iret
