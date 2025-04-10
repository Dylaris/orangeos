%include "asm/sconst.inc"

; Used in user space
; _NR_ is the index of system call in sys_call_table

_NR_get_ticks       equ 0
INT_VECTOR_SYS_CALL equ 0x90

; Export function
global get_ticks

[bits 32]
[section .text]

get_ticks:
    mov eax, _NR_get_ticks
    int INT_VECTOR_SYS_CALL
    ret