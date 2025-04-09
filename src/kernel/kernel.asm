%include "sconst.inc"

; Export function
extern cstart
extern exception_handler
extern spurious_irq
extern kernel_main
extern disp_str
extern delay
extern clock_handler

; Import global variable
extern disp_pos
extern gdt_ptr
extern idt_ptr
extern tss
extern p_proc_ready
extern k_reenter

[bits 32]

[section .data]
clock_int_msg:  db "^"

[section .bss]
stack_space:    resb 2 * 1024
stack_top:

[section .text]

global _start

; Execption handler
global divide_error
global single_step_exception
global nmi
global breakpoint_exception
global overflow
global bounds_check
global inval_opcode
global copr_not_available
global double_fault
global copr_seg_overrun
global inval_tss
global segment_not_present
global stack_exception
global general_protection
global page_fault
global copr_error

; Hardware interrupt 
global hwint00
global hwint01
global hwint02
global hwint03
global hwint04
global hwint05
global hwint06
global hwint07
global hwint08
global hwint09
global hwint10
global hwint11
global hwint12
global hwint13
global hwint14
global hwint15

global restart

_start:
    mov esp, stack_top
    mov dword [disp_pos], 0

    sgdt [gdt_ptr]
    call cstart
    lgdt [gdt_ptr]
    lidt [idt_ptr]

    jmp SELECTOR_KERNEL_CS:csinit

csinit:
    xor eax, eax
    mov ax, SELECTOR_TSS
    ltr ax
    jmp kernel_main

; Hardware interrupt
; ---------------------------------
%macro  hwint_master    1
        push    %1
        call    spurious_irq
        add     esp, 4
        hlt
%endmacro
; ---------------------------------

ALIGN   16
hwint00:                ; Interrupt routine for irq 0 (the clock).
        ; Save context
        call save

        ; Disable clock interrupt
        in al, INT_M_CTLMASK
        or al, 1
        out INT_M_CTLMASK, al

        ; Enable reenter
        mov al, EOI
        out INT_M_CTL, al

        ; Interrupt handler
        sti
        push 0
        call clock_handler
        add esp, 4
        cli

        ; Enable clock interrupt
        in al, INT_M_CTLMASK
        and al, 0xFE
        out INT_M_CTLMASK, al

        ; Restore context throuh the pushed address by 'call save'
        ret

ALIGN   16
hwint01:                ; Interrupt routine for irq 1 (keyboard)
        hwint_master    1

ALIGN   16
hwint02:                ; Interrupt routine for irq 2 (cascade!)
        hwint_master    2

ALIGN   16
hwint03:                ; Interrupt routine for irq 3 (second serial)
        hwint_master    3

ALIGN   16
hwint04:                ; Interrupt routine for irq 4 (first serial)
        hwint_master    4

ALIGN   16
hwint05:                ; Interrupt routine for irq 5 (XT winchester)
        hwint_master    5

ALIGN   16
hwint06:                ; Interrupt routine for irq 6 (floppy)
        hwint_master    6

ALIGN   16
hwint07:                ; Interrupt routine for irq 7 (printer)
        hwint_master    7

; ---------------------------------
%macro  hwint_slave     1
        push    %1
        call    spurious_irq
        add     esp, 4
        hlt
%endmacro
; ---------------------------------

ALIGN   16
hwint08:                ; Interrupt routine for irq 8 (realtime clock).
        hwint_slave     8

ALIGN   16
hwint09:                ; Interrupt routine for irq 9 (irq 2 redirected)
        hwint_slave     9

ALIGN   16
hwint10:                ; Interrupt routine for irq 10
        hwint_slave     10

ALIGN   16
hwint11:                ; Interrupt routine for irq 11
        hwint_slave     11

ALIGN   16
hwint12:                ; Interrupt routine for irq 12
        hwint_slave     12

ALIGN   16
hwint13:                ; Interrupt routine for irq 13 (FPU exception)
        hwint_slave     13

ALIGN   16
hwint14:                ; Interrupt routine for irq 14 (AT winchester)
        hwint_slave     14

ALIGN   16
hwint15:                ; Interrupt routine for irq 15
        hwint_slave     15

; Exception
divide_error:
    push 0xFFFFFFFF     ; no err code
    push 0              ; vector_no = 0
    jmp exception
single_step_exception:
    push 0xFFFFFFFF     ; no err code
    push 1              ; vector_no = 1
    jmp exception
nmi:
    push 0xFFFFFFFF     ; no err code
    push 2              ; vector_no = 2
    jmp exception
breakpoint_exception:
    push 0xFFFFFFFF     ; no err code
    push 3              ; vector_no = 3
    jmp exception
overflow:
    push 0xFFFFFFFF     ; no err code
    push 4              ; vector_no = 4
    jmp exception
bounds_check:
    push 0xFFFFFFFF     ; no err code
    push 5              ; vector_no = 5
    jmp exception
inval_opcode:
    push 0xFFFFFFFF     ; no err code
    push 6              ; vector_no = 6
    jmp exception
copr_not_available:
    push 0xFFFFFFFF     ; no err code
    push 7              ; vector_no = 7
    jmp exception
double_fault:
    push 8              ; vector_no = 8
    jmp exception
copr_seg_overrun:
    push 0xFFFFFFFF     ; no err code
    push 9              ; vector_no = 9
    jmp exception
inval_tss:
    push 10             ; vector_no = A
    jmp exception
segment_not_present:
    push 11             ; vector_no = B
    jmp exception
stack_exception:
    push 12             ; vector_no = C
    jmp exception
general_protection:
    push 13             ; vector_no = D
    jmp exception
page_fault:
    push 14             ; vector_no = E
    jmp exception
copr_error:
    push 0xFFFFFFFF     ; no err code
    push 16             ; vector_no = 10h
    jmp exception

exception:
    call exception_handler
    add esp, 4*2
    hlt

; For a typical function, the esp before and after the call remains the same. 
; We simply push the address of the instruction following the call onto the stack, and then use ret to return.
; However, for the save function, it only performs the task of saving the context 
; and stores the address of the routine that will restore the context after the interrupt ends. 
; It does not restore the esp itself. In simpler terms, the esp changes before and after calling save. 
; You can think of save and restart as two parts of a whole (i.e., saving the context and restoring it). 
; However, an interrupt must occur between them, which is why they are separated. 
; The ret instruction that ultimately returns is in the restart function, which makes the save function appear unique.
save:
        ;;;; ESP point to PCB (stack_frame->retaddr)
        pushad
        push ds
        push es
        push fs
        push gs
        mov dx, ss
        mov ds, dx
        mov es, dx

        mov eax, esp    ; Start address of PCB

        inc dword [k_reenter]
        cmp dword [k_reenter], 0
        jne .1

        ;;;; ESP point to kernel stack
        mov esp, stack_top  ; Switch to kernel stack

        push restart
        jmp [eax + RETADR - P_STACKBASE]

.1:     ; Interrupt reenter 
        push restart_reenter
        jmp [eax + RETADR - P_STACKBASE]

restart:
    ;;;; ESP point to PCB (lowest address <=> start of stack_frame)
    mov esp, [p_proc_ready]     ; Leave kernel stack and switch to PCB
    lldt [esp + P_LDT_SEL]
    lea eax, [esp + P_STACKTOP]
    mov dword [tss + TSS3_S_SP0], eax   ; Equal to tss.esp0 = stack_frame.ss
                                        ; point to the top of pushed regs in PCB
restart_reenter:
    dec dword [k_reenter]
    pop gs
    pop fs
    pop es
    pop ds
    popad

    add esp, 4

    iretd