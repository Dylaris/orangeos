%include "pm.inc"

; org     07c00h
org     0100h
    jmp LABEL_BEGIN

[SECTION .gdt]
; GDT
;                             address   limit           attr
LABEL_GDT:         Descriptor 0,        0,              0             ; null descriptor
LABEL_DESC_NORMAL: Descriptor 0,        0ffffh,         DA_DRW        ; normal descriptor
LABEL_DESC_CODE32: Descriptor 0,        SegCode32Len-1, DA_C+DA_32    ; non-consistent code segment (32 bit)
LABEL_DESC_CODE16: Descriptor 0,        0ffffh,         DA_C          ; non-consistent code segment (16 bit)
LABEL_DESC_DATA:   Descriptor 0,        DataLen-1,      DA_DRW        ; data segment (32 bit)
LABEL_DESC_STACK:  Descriptor 0,        TopOfStack,     DA_DRWA+DA_32 ; stack segment (32 bit)
LABEL_DESC_TEST:   Descriptor 0500000h, 0ffffh,         DA_DRW
LABEL_DESC_VIDEO:  Descriptor 0b8000h,  0ffffh,         DA_DRW        ; video segment
; End of GDT

; GDT Register
GdtLen  equ $ - LABEL_GDT    ; length of GDT
GdtPtr  dw  GdtLen - 1       ; limit of GDT
        dd  0                ; address of GDT
; End of GDT Register

; GDT Selector
SelectorNormal  equ LABEL_DESC_NORMAL - LABEL_GDT
SelectorCode32  equ LABEL_DESC_CODE32 - LABEL_GDT
SelectorCode16  equ LABEL_DESC_CODE16 - LABEL_GDT
SelectorData    equ LABEL_DESC_DATA   - LABEL_GDT
SelectorStack   equ LABEL_DESC_STACK  - LABEL_GDT
SelectorTest    equ LABEL_DESC_TEST   - LABEL_GDT
SelectorVideo   equ LABEL_DESC_VIDEO  - LABEL_GDT
; End of GDT Selector
; END OF [SECTION .gdt]

[SECTION .data1]
ALIGN 32
[BITS 32]
LABEL_SEG_DATA:
SPValueInRealMode   dw 0
; String
PMMessage:          db "In Protect Mode now. ^-^", 0
OffsetPMMessage     equ PMMessage - $$
StrTest:            db "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 0
OffsetStrTest       equ StrTest - $$
DataLen             equ $ - LABEL_SEG_DATA
; END OF [SECTION .data1]

; Global Stack Segment
[SECTION .gs]
ALIGN 32
[BITS 32]
LABEL_SEG_STACK:
    times 512 db 0
TopOfStack  equ $ - LABEL_SEG_STACK - 1
; END OF [SECTION .gs]

[SECTION .s16]
[BITS 16]
LABEL_BEGIN:
    mov     ax, cs
    mov     ds, ax
    mov     es, ax
    mov     ss, ax
    mov     sp, 0100h

    mov     [LABEL_GO_BACK_TO_REAL + 3], ax
    mov     [SPValueInRealMode], sp

    ; Init 16 bit code segment descriptor
    xor     eax, eax
    mov     ax, cs
    shl     eax, 4
    add     eax, LABEL_SEG_CODE16
    mov     word [LABEL_DESC_CODE16 + 2], ax
    shr     eax, 16
    mov     byte [LABEL_DESC_CODE16 + 4], al
    mov     byte [LABEL_DESC_CODE16 + 7], ah

    ; Init 32 bit code segment descriptor
    xor     eax, eax
    mov     ax, cs
    shl     eax, 4
    add     eax, LABEL_SEG_CODE32
    mov     word [LABEL_DESC_CODE32 + 2], ax
    shr     eax, 16
    mov     byte [LABEL_DESC_CODE32 + 4], al
    mov     byte [LABEL_DESC_CODE32 + 7], ah

    ; Init 32 bit data segment descriptor
    xor     eax, eax
    mov     ax, ds
    shl     eax, 4
    add     eax, LABEL_SEG_DATA
    mov     word [LABEL_DESC_DATA + 2], ax
    shr     eax, 16
    mov     byte [LABEL_DESC_DATA + 4], al
    mov     byte [LABEL_DESC_DATA + 7], ah

    ; Init 32 bit stack segment descriptor
    xor     eax, eax
    mov     ax, ds
    shl     eax, 4
    add     eax, LABEL_SEG_STACK
    mov     word [LABEL_DESC_STACK + 2], ax
    shr     eax, 16
    mov     byte [LABEL_DESC_STACK + 4], al
    mov     byte [LABEL_DESC_STACK + 7], ah

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

LABEL_REAL_ENTRY:   ; Back from protect mode
    mov     ax, cs
    mov     ds, ax
    mov     es, ax
    mov     ss, ax

    mov     sp, [SPValueInRealMode]

    ; Close A20
    in      al, 92h
    and     al, 11111101b
    out     92h, al

    ; Open interrupt
    sti

    ; Back to DOS
    mov     ax, 4c00h
    int     21h
; END OF [SECTION .s16]

[SECTION .s32]
[BITS 32]
LABEL_SEG_CODE32:
    ; Init segment selector
    mov     ax, SelectorData
    mov     ds, ax
    mov     ax, SelectorTest
    mov     es, ax
    mov     ax, SelectorVideo
    mov     gs, ax
    mov     ax, SelectorStack
    mov     ss, ax
    mov     esp, TopOfStack

    ; Display a string
    mov     ah, 0ch
    xor     esi, esi
    xor     edi, edi
    mov     esi, OffsetPMMessage
    mov     edi, (80*10+0)*2
    cld
.1:
    lodsb
    test    al, al
    jz      .2
    mov     [gs:edi], ax
    add     edi, 2
    jmp     .1
.2: ; End of string
    call    DispReturn
    call    TestRead
    call    TestWrite
    call    TestRead

    jmp     SelectorCode16:0

;----------------------------------------------------------------
TestRead:
    xor     esi, esi
    mov     ecx, 8
.loop:
    mov     al, [es:esi]
    call    DispAL
    inc     esi
    loop    .loop

    call    DispReturn
    ret
;End of TestRead ------------------------------------------------

;----------------------------------------------------------------
TestWrite:
    push    esi
    push    edi
    xor     esi, esi
    xor     edi, edi
    mov     esi, OffsetStrTest
    cld
.1:
    lodsb
    test    al, al
    jz      .2
    mov     [es:edi], ax
    inc     edi
    jmp     .1
.2:
    pop     edi
    pop     esi
    ret
;End of TestWrite -----------------------------------------------

;----------------------------------------------------------------
; Display number in AL
;   AL  <- number
;   EDI <- next character
;----------------------------------------------------------------
DispAL:
    push    ecx
    push    edx

    mov     ah, 0ch
    mov     dl, al
    shr     al, 4
    mov     ecx, 2
.begin:
    and     al, 01111b
    cmp     al, 9
    ja      .1
    add     al, '0'
    jmp     .2
.1:
    sub     al, 0ah
    add     al, 'A'
.2:
    mov     [gs:edi], ax
    add     edi, 2

    mov     al, dl
    loop    .begin
    add     edi, 2

    pop     edx
    pop     ecx
    ret
;End of DispAL --------------------------------------------------

;----------------------------------------------------------------
DispReturn:
    push    eax
    push    ebx
    mov     eax, edi
    mov     bl, 160
    div     bl
    and     eax, 0ffh
    inc     eax
    mov     bl, 160
    mul     bl
    mov     edi, eax
    pop     ebx
    pop     eax
    ret
;End of DispReturn ----------------------------------------------

SegCode32Len    equ $ - LABEL_SEG_CODE32
; END OF [SECTION .s32]

; Switch to real mode
[SECTION .s16code]
ALIGN 32
[BITS 16]
LABEL_SEG_CODE16:
    mov     ax, SelectorNormal
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax

    mov     eax, cr0
    and     al, 11111110b
    mov     cr0, eax

LABEL_GO_BACK_TO_REAL:
    jmp     0:LABEL_REAL_ENTRY
; END OF [SECTION .s16code]
