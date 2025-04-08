[section .text]

global memcpy
global memset

;===== void *memcpy(void *es:dst, void *ds:src, int size);
memcpy:
    push ebp
    mov ebp, esp

    push esi
    push edi
    push ecx

    mov edi, [ebp + 8]  ; Destination
    mov esi, [ebp + 12] ; Source
    mov ecx, [ebp + 16] ; Counter
.1:
    cmp ecx, 0
    jz .2

    mov al, [ds:esi]
    inc esi

    mov byte [es:edi], al
    inc edi

    dec ecx
    jmp .1
.2:
    mov eax, [ebp + 8] ; Return value

    pop ecx
    pop edi
    pop esi
    mov esp, ebp
    pop ebp

    ret

; void memset(void* dst, char ch, int size);
memset:
    push ebp
    mov ebp, esp

    push esi
    push edi
    push ecx

    mov edi, [ebp + 8]  ; Destination
    mov edx, [ebp + 12] ; Char to be putted
    mov ecx, [ebp + 16] ; Counter
.1:
    cmp ecx, 0
    jz .2

    mov byte [edi], dl
    inc edi

    dec ecx
    jmp .1
.2:

    pop ecx
    pop edi
    pop esi
    mov esp, ebp
    pop ebp

    ret