; crt0.nasm
; - Set up the stack for the barebones usermode modules.

; ===================
; Globals and Externs
; ===================

GLOBAL _start
EXTERN main

; =====================
; Program Entry (.code)
; =====================
SECTION .code
BITS 32

_start: mov  esp, _init_stack
        call main
        jmp  $

; =========================
; Default Stack Area (.bss)
; =========================
SECTION .bss
BITS 32

RESB 4096
_init_stack: