; crt0.nasm
; - Set up the stack for the barebones usermode modules.

; ===================
; Globals and Externs
; ===================

GLOBAL _start
EXTERN __malloc_initialize
EXTERN main
EXTERN kill

; =====================
; Program Entry (.code)
; =====================
SECTION .code
BITS 32

_start: mov  esp, _init_stack

		push 0x20000000
		call __malloc_initialize
		add esp, 4

        call main

        push 0
        call kill
        jmp  $

; =========================
; Default Stack Area (.bss)
; =========================
SECTION .bss
BITS 32

RESB 4096
_init_stack:
