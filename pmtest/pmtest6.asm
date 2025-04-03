%include "pm.inc"

PageDirBase     equ 200000h     ; 2M
PageTblBase     equ 201000h     ; 2M + 4K

; org     07c00h
org     0100h
    jmp LABEL_BEGIN

[SECTION .gdt]
; GDT
;                             address               limit           attr
LABEL_GDT:              Descriptor 0,               0,              0                   ; null descriptor
LABEL_DESC_NORMAL:      Descriptor 0,               0ffffh,         DA_DRW              ; normal descriptor
LABEL_DESC_CODE32:      Descriptor 0,               SegCode32Len-1, DA_C+DA_32          ; non-consistent code segment (32 bit)
LABEL_DESC_CODE16:      Descriptor 0,               0ffffh,         DA_C                ; non-consistent code segment (16 bit)
LABEL_DESC_DATA:        Descriptor 0,               DataLen-1,      DA_DRW              ; data segment (32 bit)
LABEL_DESC_STACK:       Descriptor 0,               TopOfStack,     DA_DRWA+DA_32       ; stack segment (32 bit)
LABEL_DESC_VIDEO:       Descriptor 0b8000h,         0ffffh,         DA_DRW              ; video segment
LABEL_DESC_PAGE_DIR:    Descriptor PageDirBase,     4095,           DA_DRW              ; page directory
LABEL_DESC_PAGE_TBL:    Descriptor PageTblBase,     1023,           DA_DRW|DA_LIMIT_4K  ; page tables
; End of GDT

; GDT Register
GdtLen  equ $ - LABEL_GDT    ; length of GDT
GdtPtr  dw  GdtLen - 1       ; limit of GDT
        dd  0                ; address of GDT
; End of GDT Register

; GDT Selector
SelectorNormal  equ LABEL_DESC_NORMAL   - LABEL_GDT
SelectorCode32  equ LABEL_DESC_CODE32   - LABEL_GDT
SelectorCode16  equ LABEL_DESC_CODE16   - LABEL_GDT
SelectorData    equ LABEL_DESC_DATA     - LABEL_GDT
SelectorStack   equ LABEL_DESC_STACK    - LABEL_GDT
SelectorVideo   equ LABEL_DESC_VIDEO    - LABEL_GDT
SelectorPageDir equ LABEL_DESC_PAGE_DIR - LABEL_GDT
SelectorPageTbl equ LABEL_DESC_PAGE_TBL - LABEL_GDT
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

    jmp     SelectorCode16:0

;----------------------------------------------------------------
SetupPaging:
    ; linear address = physic address

    ; Init page directory
    mov     ax, SelectorPageDir
    mov     es, ax
    mov     ecx, 1024   ; 1K PDE
    xor     edi, edi
    xor     eax, eax
    mov     eax, PageTblBase | PG_P | PG_USU | PG_RWW
.1:
    stosd
    add     eax, 4096   ; next PTE (4K)
    loop    .1

    ; Init page tables
    mov     ax, SelectorPageTbl
    mov     es, ax
    mov     ecx, 1024 * 1024    ; 1K * 1K PTE
    xor     edi, edi
    xor     eax, eax
    mov     eax, PG_P | PG_USU | PG_RWW
.2:
    stosd
    add     eax, 4096
    loop    .2

    ; Open paging
    mov     eax, PageDirBase
    mov     cr3, eax
    mov     eax, cr0
    or      eax, 80000000h
    mov     cr0, eax
    jmp     short .3
.3:
    nop
    ret
;End of SetupPaging ---------------------------------------------

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
