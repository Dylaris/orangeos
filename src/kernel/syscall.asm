%include "asm/sconst.inc"

; Used in user space
; _NR_ is the index of system call in sys_call_table

_NR_printx          equ 0
_NR_sendrec         equ 1
INT_VECTOR_SYS_CALL equ 0x90

; Export function
global printx
global sendrec

[bits 32]
[section .text]

; Never call sendrec() directly, call send_recv() instead.
sendrec:
    mov eax, _NR_sendrec
    mov ebx, [esp + 4]  ; function
    mov ecx, [esp + 8]  ; src_dest
    mov edx, [esp + 12] ; p_msg
    int INT_VECTOR_SYS_CALL
    ret

printx:
    mov eax, _NR_printx
    mov ebx, [esp + 4]
    int INT_VECTOR_SYS_CALL
    ret