%include "pm.inc"

; org     07c00h
org     0100h
    jmp LABEL_BEGIN

[SECTION .gdt]
; GDT
;                             address  limit             attr
LABEL_GDT:         Descriptor 0,       0,                0              ; null descriptor
LABEL_DESC_CODE32: Descriptor 0,       SegCode32Len - 1, DA_C + DA_32   ; non-consistent code segment
LABEL_DESC_VIDEO:  Descriptor 0b8000h, 0ffffh,           DA_DRW         ; video segment
; End of GDT

; GDT Register
GdtLen  equ $ - LABEL_GDT    ; length of GDT
GdtPtr  dw  GdtLen - 1       ; limit of GDT
        dd  0                ; address of GDT
; End of GDT Register

; GDT Selector
SelectorCode32  equ LABEL_DESC_CODE32 - LABEL_GDT
SelectorVideo   equ LABEL_DESC_VIDEO  - LABEL_GDT
; End of GDT Selector
; END OF [SECTION .gdt]

[SECTION .s16]
[BITS 16]
LABEL_BEGIN:
    mov     ax, cs
    mov     ds, ax
    mov     es, ax
    mov     ss, ax
    mov     sp, 0100h

    ; Init 32 bit code segment descriptor
    xor     eax, eax
    mov     ax, cs
    shl     eax, 4
    add     eax, LABEL_SEG_CODE32
    mov     word [LABEL_DESC_CODE32 + 2], ax
    shr     eax, 16
    mov     byte [LABEL_DESC_CODE32 + 4], al
    mov     byte [LABEL_DESC_CODE32 + 7], ah

    ; Init GdtPtr
    xor     eax, eax
    mov     ax, ds
    shl     eax, 4
    add     eax, LABEL_GDT  ; eax <- address of GDT
    mov     dword [GdtPtr + 2], eax

    ; Load GDTR
    lgdt    [GdtPtr]

    ; Close interrupt
    cli

    ; Open A20
    in      al, 92h
    or      al, 00000010b
    out     92h, al

    ; Set cr0
    mov     eax, cr0
    or      eax, 1
    mov     cr0, eax

    ; Switch to protect mode
    jmp     dword SelectorCode32:0
; END OF [SECTION .s16]

[SECTION .s32]
[BITS 32]
LABEL_SEG_CODE32:
    mov     ax, SelectorVideo
    mov     gs, ax

    mov     edi, (80 * 11 + 79) * 2     ; row: 11, column: 79
    mov     ah, 0ch ; 0000 black background, 1100 red foreground
    mov     al, 'p'
    mov     [gs:edi], ax

    jmp     $ 

SegCode32Len    equ $ - LABEL_SEG_CODE32
; END OF [SECTION .s32]
