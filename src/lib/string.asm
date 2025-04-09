[section .text]

global memcpy
global memset
global strcpy

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

;===== char *strcpy(char *dst, char *src);
strcpy:
    push ebp
    mov ebp, esp

    mov esi, [esp + 12] ; src
    mov edi, [esp + 8]  ; dst
.1
    mov al, [esi]
    inc esi

    mov byte [edi], al
    inc edi

    cmp al, 0
    jne .1

    mov eax, [ebp + 8]
    pop ebp
    ret