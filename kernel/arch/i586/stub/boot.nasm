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

; boot.nasm
; - The absolute starting point for the i586 microkernel.

; ===================
; Globals and Externs
; ===================

GLOBAL bootstrap
EXTERN kmain

EXTERN _KERNEL_PHYS_ADDR
EXTERN _KERNEL_VIRT_ADDR

; =======
; Defines
; =======

; MultiBoot Constants
MBH_PAGE_ALIGN equ 1<<0
MBH_MEM_INFO   equ 1<<1
MBH_MAGIC      equ 0x1BADB002
MBH_FLAGS      equ MBH_PAGE_ALIGN | MBH_MEM_INFO
MBH_CHECKSUM   equ -(MBH_MAGIC + MBH_FLAGS)

; Bootloader Constants
BL_PGDIR       equ 0x9D000
BL_PGTBL_ID    equ 0x9E000
BL_PGTBL_3G    equ 0x9F000
BL_PGTBL_FLAGS equ 0x3

; ==================
; Bootstrap (.setup)
; ==================
SECTION .setup
BITS 32

bootstrap: push eax ; Save MultiBoot magic
           push ebx ; Save MultiBoot structure pointer

           ; Setup page table for low memory identity mapping
           mov  eax, BL_PGTBL_ID
           mov  ebx, 0x00000000
           or   ebx, BL_PGTBL_FLAGS
           call map

           ; Calculate the virtual offset to map the kernel
           lea  ecx, [_KERNEL_PHYS_ADDR]
           lea  edx, [_KERNEL_VIRT_ADDR]
           sub  edx, 768<<22
           sub  ecx, edx

           ; Setup page table for high memory kernel addressing
           mov  eax, BL_PGTBL_3G
           mov  ebx, ecx
           or   ebx, BL_PGTBL_FLAGS
           call map

           ; Format the page directory
           mov  dword [BL_PGDIR +    0*4], BL_PGTBL_ID | BL_PGTBL_FLAGS
           mov  dword [BL_PGDIR +  768*4], BL_PGTBL_3G | BL_PGTBL_FLAGS
           mov  dword [BL_PGDIR + 1023*4], BL_PGDIR    | BL_PGTBL_FLAGS ; Setup recursive mapping

           ; Enable paging
           mov  eax, BL_PGDIR
           mov  cr3, eax        ; cr3 contains the pointer to the active page directory
           mov  eax, cr0
           or   eax, 0x80000000 ; Set the 'Paging Enable' flag bit
           mov  cr0, eax

           ; Restore the MultiBoot information
           pop  ebx
           pop  eax

           ; Setup the maximum mapped address for kmain
           mov  ecx, 768<<22   ; The start address of kernel high memory (3GB)
           add  ecx, 4096*1024 ; The mapped region of one page table.

           ; Jump to kernel main
           lea  edx, [kstart]
           jmp  edx

; Routine: Setup a page directory entry for a page table.
;	EAX: The page table's index in the page directory
;	EBX: The page table's physical address OR'd with any paging flags
; -----------------------------------------------------------------
map:       mov  ecx, 1024        ; Set the loop range; map every PTE in the page table
.loop      mov  dword [eax], ebx ; Set the table frame in the entry
           add  eax, 4           ; Move to the next entry
           add  ebx, 4096        ; Move to the next physical page
           loop .loop
           ret

; ================
; MultiBoot Header
; ================
BITS  32
ALIGN 4

multiboot: dd   MBH_MAGIC
           dd   MBH_FLAGS
           dd   MBH_CHECKSUM

; ======================
; Kernel Entry (.kentry)
; ======================
SECTION .kentry
BITS 32

kstart:    mov  esp, stack

           ; Pass the boot parameters to the kernel main C function
           push ecx
           push ebx
           push eax

           call kmain
           hlt        ; This instruction should never be reached

; ==========================
; Default Stack [4KB] (.bss)
; ==========================
SECTION .bss
BITS 32

RESB 4096
stack:    ; Stack grows backwards, so point to the end of the area
