; Type of descriptor
DA_32       equ 4000h   ; 32 bit segment
DA_LIMIT_4K equ 8000h

DA_DPL0     equ 00h     ; DPL = 0
DA_DPL1     equ 20h     ; DPL = 1
DA_DPL2     equ 40h     ; DPL = 2
DA_DPL3     equ 60h     ; DPL = 3

; Type of storage segment descriptor
DA_DR       equ 90h     ; data segment (present, read)
DA_DRW      equ 92h     ; data segment (present, read, write)
DA_DRWA     equ 93h     ; data segment (present, read, write, accessed)
DA_C        equ 98h     ; code segment (present, execute)
DA_CR       equ 9ah     ; code segment (present, execute, read)
DA_CCO      equ 9ch     ; code segment (present, execute, consistent)
DA_CCOR     equ 9eh     ; code segment (present, execute, consistent, read)

; Type of system segment descriptor
DA_LDT      equ 82h     ; LDT
DA_TaskGate equ 85h     ; task gate
DA_386TSS   equ 89h     ; available 386 task status
DA_386CGate equ 8ch     ; 386 call gate
DA_386IGate equ 8eh     ; 386 interrupt gate
DA_386TGate equ 8fh     ; 386 trap gate

; Type of selector
SA_RPL0     equ 0
SA_RPL1     equ 1
SA_RPL2     equ 2
SA_RPL3     equ 3

SA_TIG      equ 0
SA_TIL      equ 4

; Paging
PG_P        equ 1       ; present
PG_RWR      equ 0       ; read/execute
PG_RWW      equ 2       ; read/write/execute
PG_USS      equ 0       ; superisor
PG_USU      equ 4       ; user

; Descriptor
; Usage: Descriptor Base, Limit, Attr
;        Base:  dd (32 bits)
;        Limit: dd (low 20 bits available)
;        Attr:  dw (lower 4 bits of higher byte are always 0)
; Total 8 bytes
%macro Descriptor 3
    dw  %2 & 0ffffh                             ; limit 1
    dw  %1 & 0ffffh                             ; address 1
    db  (%1 >> 16) & 0ffh                       ; address 2
    dw  ((%2 >> 8) & 0f00h) | (%3 & 0f0ffh)     ; attribute 1 + limit 2 + attribute 2
    db  (%1 >> 24) & 0ffh                       ; address 3
%endmacro

; Gate
; Usage: Gate Selector, Offset, DCount, Attr
;        Selector:  dw
;        Offset:    dd
;        DCount:    db
;        Attr:      db
; Total 8 bytes
%macro Gate 4
	dw	(%2 & 0FFFFh)				        ; offset 1
	dw	%1					                ; selector
	dw	(%3 & 1Fh) | ((%4 << 8) & 0FF00h)	; attribute
	dw	((%2 >> 16) & 0FFFFh)			    ; offset 2
%endmacro