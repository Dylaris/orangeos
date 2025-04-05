SELECTOR_KERNEL_CS equ 8

extern cstart   ; Import c function
extern gdt_ptr  ; Import global variable

[section .bss]

StackSpace resb 2 * 1024    ; 2KB stack
StackTop: 

[section .text]

global _start   ; Export _start

_start:
    mov esp, StackTop   ; Switch to kernel stack

    sgdt [gdt_ptr]      ; cstart() will use gdt_ptr
    call cstart         ; Change gdt_ptr and let it point to new GDT
    lgdt [gdt_ptr]      ; Switch to new GDT

    jmp SELECTOR_KERNEL_CS:csinit   ; Force to use new GDT

csinit:
    push 0
    popfd ; Pop top of stack into EFLAGS

    hlt
