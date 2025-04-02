%include "pm.inc"

; org     07c00h
org     0100h
    jmp LABEL_BEGIN

[SECTION .gdt]
; GDT
;                                address   limit                attr
LABEL_GDT:              Descriptor 0,        0,                 0                       ; null descriptor
LABEL_DESC_NORMAL:      Descriptor 0,        0ffffh,            DA_DRW                  ; normal descriptor
LABEL_DESC_CODE32:      Descriptor 0,        SegCode32Len-1,    DA_C+DA_32              ; non-consistent code segment (32 bit, DPL=0)
LABEL_DESC_CODE16:      Descriptor 0,        0ffffh,            DA_C                    ; non-consistent code segment (16 bit, DPL=0)
LABEL_DESC_DATA:        Descriptor 0,        DataLen-1,         DA_DRW                  ; data segment (limit is 4G, DPL=0) 
LABEL_DESC_STACK:       Descriptor 0,        TopOfStack,        DA_DRWA+DA_32           ; stack segment (use esp, DPL=0)
LABEL_DESC_LDT:         Descriptor 0,        LDTLen-1,          DA_LDT                  ; LDT descriptor (DPL=0)
LABEL_DESC_VIDEO:       Descriptor 0b8000h,  0ffffh,            DA_DRW+DA_DPL3          ; video segment (DPL=3)
LABEL_DESC_CODE_DEST:   Descriptor 0,        SegCodeDestLen-1,  DA_C+DA_32              ; non-consistent code segment (32 bit)
LABEL_DESC_CODE_RING3:  Descriptor 0,        SegCodeRing3Len-1, DA_C+DA_32+DA_DPL3      ; non-consistent code segment (32 bit, DPL=3)
LABEL_DESC_STACK3:      Descriptor 0,        TopOfStack3,       DA_DRWA+DA_32+DA_DPL3   ; stack segment (use esp, DPL=3)
LABEL_DESC_TSS:         Descriptor 0,        TSSLen-1,          DA_386TSS               ; tss segment
;                          selector          offset DCount  attr   
LABEL_CALL_GATE_TEST: Gate SelectorCodeDest, 0,     0,      DA_386CGate+DA_DPL3         ; call gate segment (DPL=3)
; End of GDT

; GDT Register
GdtLen  equ $ - LABEL_GDT    ; length of GDT
GdtPtr  dw  GdtLen - 1       ; limit of GDT
        dd  0                ; address of GDT
; End of GDT Register

; GDT Selector
SelectorNormal          equ LABEL_DESC_NORMAL       - LABEL_GDT
SelectorCode32          equ LABEL_DESC_CODE32       - LABEL_GDT
SelectorCode16          equ LABEL_DESC_CODE16       - LABEL_GDT
SelectorData            equ LABEL_DESC_DATA         - LABEL_GDT
SelectorStack           equ LABEL_DESC_STACK        - LABEL_GDT
SelectorLDT             equ LABEL_DESC_LDT          - LABEL_GDT
SelectorVideo           equ LABEL_DESC_VIDEO        - LABEL_GDT
SelectorCodeDest        equ LABEL_DESC_CODE_DEST    - LABEL_GDT
SelectorCallGateTest    equ LABEL_CALL_GATE_TEST    - LABEL_GDT + SA_RPL3
SelectorCodeRing3       equ LABEL_DESC_CODE_RING3   - LABEL_GDT + SA_RPL3
SelectorStack3          equ LABEL_DESC_STACK3       - LABEL_GDT + SA_RPL3
SelectorTSS             equ LABEL_DESC_TSS          - LABEL_GDT
; End of GDT Selector
; END OF [SECTION .gdt]

[SECTION .tss]
ALIGN 32
[BITS 32]
LABEL_TSS:
    DD  0               ; Back
    DD  TopOfStack      ; stack (ring 0)
    DD  SelectorStack
    DD  0               ; Stack (ring 1)
    DD  0
    DD  0               ; Stack (ring 2)
    DD  0
    DD  0               ; CR3
    DD  0               ; EIP
    DD  0               ; EFALGS
    DD  0               ; EAX
    DD  0               ; ECX
    DD  0               ; EDX
    DD  0               ; EBX
    DD  0               ; EAX
    DD  0               ; ECX
    DD  0               ; EDX
    DD  0               ; EBX
    DD  0               ; ESP
    DD  0               ; EBP
    DD  0               ; ESI
    DD  0               ; EDI
    DD  0               ; ES
    DD  0               ; CS
    DD  0               ; SS
    DD  0               ; DS
    DD  0               ; FS
    DD  0               ; GS
    DD  0               ; LDT selector
    DW  0               ; Trap flag
    DW  $-LABEL_TSS+2   ; Address of I/O bit map
    DB  0ffh            ; End flag of I/O bit map

TSSLen  equ $ - LABEL_TSS
; END OF [SECTION .tss]

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

; Stack for ring 3
[SECTION .s3]
ALIGN 32
[BITS 32]
LABEL_STACK3:
    times 512 db 0
TopOfStack3 equ $ - LABEL_STACK3 - 1
; END OF [SECTION .s3]

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

    ; Init 32 bit code segment (code dest for call gate) descriptor
    xor     eax, eax
    mov     ax, ds
    shl     eax, 4
    add     eax, LABEL_SEG_CODE_DEST
    mov     word [LABEL_DESC_CODE_DEST + 2], ax
    shr     eax, 16
    mov     byte [LABEL_DESC_CODE_DEST + 4], al
    mov     byte [LABEL_DESC_CODE_DEST + 7], ah

    ; Init tss segment
    xor     eax, eax
    mov     ax, ds
    shl     eax, 4
    add     eax, LABEL_TSS
    mov     word [LABEL_DESC_TSS + 2], ax
    shr     eax, 16
    mov     byte [LABEL_DESC_TSS + 4], al
    mov     byte [LABEL_DESC_TSS + 7], ah

    ; Init LDT descriptor
    xor     eax, eax
    mov     ax, ds
    shl     eax, 4
    add     eax, LABEL_LDT
    mov     word [LABEL_DESC_LDT + 2], ax
    shr     eax, 16
    mov     byte [LABEL_DESC_LDT + 4], al
    mov     byte [LABEL_DESC_LDT + 7], ah

    ; Init LDT code segment descriptor
    xor     eax, eax
    mov     ax, ds
    shl     eax, 4
    add     eax, LABEL_CODE_A
    mov     word [LABEL_LDT_DESC_CODEA + 2], ax
    shr     eax, 16
    mov     byte [LABEL_LDT_DESC_CODEA + 4], al
    mov     byte [LABEL_LDT_DESC_CODEA + 7], ah

    ; Init code segment descriptor (ring 3)
    xor     eax, eax
    mov     ax, ds
    shl     eax, 4
    add     eax, LABEL_CODE_RING3
    mov     word [LABEL_DESC_CODE_RING3 + 2], ax
    shr     eax, 16
    mov     byte [LABEL_DESC_CODE_RING3 + 4], al
    mov     byte [LABEL_DESC_CODE_RING3 + 7], ah

    ; Init stack segment descriptor (ring 3)
    xor     eax, eax
    mov     ax, ds
    shl     eax, 4
    add     eax, LABEL_STACK3
    mov     word [LABEL_DESC_STACK3 + 2], ax
    shr     eax, 16
    mov     byte [LABEL_DESC_STACK3 + 4], al
    mov     byte [LABEL_DESC_STACK3 + 7], ah

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
    call    DispReturn

    ; Load tss
    mov     ax, SelectorTSS
    ltr     ax

    ; Jump into code (ring3)
    push    SelectorStack3      ; ss
    push    TopOfStack3         ; esp
    push    SelectorCodeRing3   ; cs
    push    0                   ; eip
    retf

    ; Load LDT
    mov     ax, SelectorLDT
    lldt    ax

    ; Jump into local task
    jmp     SelectorLDTCodeA:0

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

[SECTION .ldt]
; LDT
LABEL_LDT:
;                                address   limit           attr
LABEL_LDT_DESC_CODEA: Descriptor 0,        CodeALen-1,     DA_C+DA_32 ; code segment (32 bit)

LDTLen  equ $ - LABEL_LDT
; End of LDT

; LDT Selector
SelectorLDTCodeA    equ LABEL_LDT_DESC_CODEA - LABEL_LDT + SA_TIL
; End of LDT Selector
; END OF [SECTION .ldt]

; CodeA (LDT 32bit code segment)
[SECTION .la]
ALIGN 32
[BITS 32]
LABEL_CODE_A:
    mov     ax, SelectorVideo
    mov     gs, ax

    mov     edi, (80*12+0)*2
    mov     ah, 0ch
    mov     al, 'L'
    mov     [gs:edi], ax

    ; Back to real mode
    jmp     SelectorCode16:0

CodeALen    equ $ - LABEL_CODE_A
; END OF [SECTION .la]

; The target segment of call gate
[SECTION .sdest]
[BITS 32]
LABEL_SEG_CODE_DEST:
    mov     ax, SelectorVideo
    mov     gs, ax

    mov     edi, (80*13+0)*2
    mov     ah, 0ch
    mov     al, 'C'
    mov     [gs:edi], ax

    retf

SegCodeDestLen  equ $ - LABEL_SEG_CODE_DEST
; END OF [SECTION .sdest]

; Code for ring 3
[SECTION .ring3]
ALIGN 32
[BITS 32]
LABEL_CODE_RING3:
    mov     ax, SelectorVideo
    mov     gs, ax

    mov     edi, (80*14+0)*2
    mov     ah, 0ch
    mov     al, '3'
    mov     [gs:edi], ax

    call    SelectorCallGateTest:0

    jmp     $

SegCodeRing3Len     equ $ - LABEL_CODE_RING3
; END OF [SECTION .ring3]
