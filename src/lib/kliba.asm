%include "asm/sconst.inc"

; Import global variable
extern disp_pos

[section .text]
; Export function
global disp_str
global disp_color_str
global out_byte
global in_byte
global disable_irq
global enable_irq
global disable_int
global enable_int
global port_read
global port_write

;===== void disp_str(char *str);
disp_str:
    push ebp
    mov ebp, esp

    push esi
    push edi
    push ebx

    mov esi, [ebp + 8] ; str
    mov edi, [disp_pos]
    mov ah, 0Fh
.1:
    lodsb
    test al, al
    jz .2
    cmp al, 0Ah ; is newline?
    jnz .3
    push eax
    mov eax, edi
    mov bl, 160
    div bl
    and eax, 0FFh
    inc eax
    mov bl, 160
    mul bl
    mov edi, eax
    pop eax
    jmp .1
.3:
    mov [gs:edi], ax
    add edi, 2
    jmp .1

.2:
    mov [disp_pos], edi

    pop ebx
    pop edi
    pop esi
    pop ebp
    ret

;===== void disp_color_str(char *str, int color);
disp_color_str:
    push ebp
    mov ebp, esp

    push esi
    push edi
    push ebx

    mov esi, [ebp + 8]      ; str
    mov edi, [disp_pos]
    mov ah, [ebp + 8 + 4]   ; color
.1:
    lodsb
    test al, al
    jz .2
    cmp al, 0Ah ; is newline?
    jnz .3
    push eax
    mov eax, edi
    mov bl, 160
    div bl
    and eax, 0FFh
    inc eax
    mov bl, 160
    mul bl
    mov edi, eax
    pop eax
    jmp .1
.3:
    mov [gs:edi], ax
    add edi, 2
    jmp .1

.2:
    mov [disp_pos], edi

    pop ebx
    pop edi
    pop esi
    pop ebp
    ret

;===== void out_byte(u16 port, u8 value);
out_byte:
    mov edx, [esp + 4]      ; port
    mov al, [esp + 4 + 4]   ; value
    out dx, al
    nop
    nop
    ret

;===== u8 in_byte(u16 port);
in_byte:
    mov edx, [esp + 4] ; port
    xor eax, eax
    in al, dx
    nop
    nop
    ret

;===== void disable_irq(int irq);
;   if (irq < 8)     
;       out_byte(INT_M_CTLMASK, in_byte(INT_M_CTLMASK) | (1 << irq));
;   else
;       out_byte(INT_S_CTLMASK, in_byte(INT_S_CTLMASK) | (1 << irq));
disable_irq:
    mov ecx, [esp + 4]          ; irq
    pushf
    cli
    mov ah, 1
    rol ah, cl                  ; ah = (1 << (irq % 8))
    cmp cl, 8
    jae disable_8               ; disable irq >= 8 at the slave 8259

disable_0:
    in al, INT_M_CTLMASK
    test al, ah
    jnz dis_already             ; already disabled?
    or al, ah
    out INT_M_CTLMASK, al       ; set bit at master 8259
    popf
    mov eax, 1                  ; disabled by this function
    ret

disable_8:
    in al, INT_S_CTLMASK
    test al, ah
    jnz dis_already             ; already disabled?
    or al, ah
    out INT_S_CTLMASK, al       ; set bit at slave 8259
    popf
    mov eax, 1                  ; disabled by this function
    ret

dis_already:
    popf
    xor eax, eax                ; already disabled
    ret

;===== void enable_irq(int irq);
;   if (irq < 8)     
;       out_byte(INT_M_CTLMASK, in_byte(INT_M_CTLMASK) & ~(1 << irq));
;   else
;       out_byte(INT_S_CTLMASK, in_byte(INT_S_CTLMASK) & ~(1 << irq));
enable_irq:
    mov ecx, [esp + 4]          ; irq
    pushf
    cli
    mov ah, ~1
    rol ah, cl                  ; ah = ~(1 << (irq % 8))
    cmp cl, 8
    jae enable_8                ; enable irq >= 8 at the slave 8259

enable_0:
    in al, INT_M_CTLMASK
    and al, ah
    out INT_M_CTLMASK, al       ; clear bit at master 8259
    popf
    ret

enable_8:
    in al, INT_S_CTLMASK
    and al, ah
    out INT_S_CTLMASK, al       ; clear bit at slave 8259
    popf
    ret


;===== void disable_int(void);
disable_int:
    cli
    ret

;===== void enable_int(void);
enable_int:
    sti
    ret

;===== void port_read(u16 port, void *buf, int n);
port_read:
    mov edx, [esp + 4]          ; port
    mov edi, [esp + 4 + 4]      ; buf
    mov ecx, [esp + 4 + 4 + 4]  ; n
    shr ecx, 1
    cld
    rep insw
    ret

;===== void port_write(u16 port, void *buf, int n);
port_write:
    mov edx, [esp + 4]          ; port
    mov edi, [esp + 4 + 4]      ; buf
    mov ecx, [esp + 4 + 4 + 4]  ; n
    shr ecx, 1
    cld
    rep outsw
    ret