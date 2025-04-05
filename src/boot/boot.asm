%ifdef _BOOT_DEBUG_
	org 0100h  ; 调试状态，做成 .com 文件，可调式
%else
	org 07c00h ; Boot 状态，BIOS 将把 Boot Sector(引导扇区) 加载到 0:7C00 处并执行
%endif

;===============================================================================
%ifdef _BOOT_DEBUG_
    BaseOfStack     equ 0100h  ; DEBUT 状态下的堆栈基地址
%else
    BaseOfStack     equ 07c00h ; 堆栈基地址
%endif

BaseOfLoader    equ 09000h  ; LOADER.BIN 被加载到的位置 —— 段地址
OffsetOfLoader  equ 0100h   ; LOADER.BIN 被加载到的位置 —— 偏移地址
;===============================================================================

    jmp short LABEL_START       ; 跳转到代码开始执行的位置
    nop                         ; 不知道干啥用，但是不能少

; 引入 FAT12 磁盘的头
%include    "include/fat12hdr.inc"
%include    "include/load.inc"

LABEL_START:
    mov     ax, cs
    mov     ds, ax
    mov     es, ax
    mov     ss, ax
    mov     sp, BaseOfStack

    ; 清屏
    mov     ax, 0600h
    mov     bx, 0700h
    mov     cx, 0       ; 左上角(0, 0)
    mov     dx, 0184fh  ; 右下角(80, 50)
    int     10h

    mov     dh, 0       ; "Booting  "
    call    DispStr     ; 显示字符串

    xor     ah, ah  
    xor     dl, dl  ; 软驱复位
    int     13h      

; 在 A 盘的根目录寻找 Loader.bin
    mov     word [wSectorNo], SectorNoOfRootDirectory
LABEL_SEARCH_IN_ROOT_DIR_BEGIN:
    cmp     word [wRootDirSizeForLoop], 0   ; 判断根目录区是否已读完
    jz      LABEL_NO_LOADERBIN              ; 若读完，则表示没有找到 LOADER.BIN
    dec     word [wRootDirSizeForLoop]      ; 根目录占用的扇区数 -1
    mov     ax, BaseOfLoader
    mov     es, ax                          ; es <- BaseOfLoader
    mov     bx, OffsetOfLoader              ; bx <- OffsetOfLoader
    mov     ax, [wSectorNo]                 ; ax <- Root Director 中的某 Sector 号
    mov     cl, 1
    call    ReadSector                      ; 执行后，加载的结束根目录区中第 wSectorNo 号扇区的数据（加载位置：es:bx）

    mov     si, LoaderFileName              ; ds:si -> "LOADER  BIN"
    mov     di, OffsetOfLoader              ; es:di -> BaseOfLoader:0100
    cld
    mov     dx, 10h                         ; 根目录每个扇区的最大文件数(条目) = 512(一个扇区大小) / 32(条目大小) = 16 = 10H
LABEL_SEARCH_FOR_LOADERBIN:                 ; 遍历扇区中的每个条目
    cmp     dx, 0                           ; 循环次数
    jz      LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR ; 若已经读完了一个扇区，就跳转到下一个扇区
    dec     dx                              ; 循环次数 -1
    mov     cx, 11                          ; 文件名称长度
LABEL_CMP_FILENAME:                         ; 比较文件名
    cmp     cx, 0
    jz      LABEL_FILENAME_FOUND            ; 若文件名相等，则表示找到
    dec     cx                              ; 循环次数 -1
    lodsb                                   ; ds:si -> al
    cmp     al, byte [es:di]
    jz      LABEL_GO_ON                     ; 若字符相同，则继续循环
    jmp     LABEL_DIFFERENT                 ; 字符不相同，表示该条目不是我们要找的

LABEL_GO_ON:
    inc     di
    jmp     LABEL_CMP_FILENAME              ; 继续循环

LABEL_DIFFERENT:                            ; 跳转到下一个条目，然后重新开始比较字符串
    and     di, 0FFE0h                      ; di &= E0 是为了让它指向本条目的开头
    add     di, 20h                         ; di += 20h 表示下一个目录条目
    mov     si, LoaderFileName
    jmp     LABEL_SEARCH_FOR_LOADERBIN

LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR:         ; 在根目录区内跳转到下一个扇区
    add     word [wSectorNo], 1             ; 要读取的扇区号 +1
    jmp     LABEL_SEARCH_IN_ROOT_DIR_BEGIN  ; 重新去读取这个扇区

LABEL_NO_LOADERBIN:     ; 找不到 LOADER.BIN
    mov     dh, 2       ; 字符串：No LOADER.
    call DispStr        ; 显示字符串

%ifdef _BOOT_DEBUG_
    mov     ax, 4c00h
    int     21h         ; DEBUG 状态下，没有找到 LOADER.BIN 则回到 DOS
%else
    jmp     $           ; 没有找到 LOADER.BIN 则死循环在此处
%endif

LABEL_FILENAME_FOUND:   ; 找到 LOADER.BIN 后，便来到这里。
    mov     ax, RootDirSectors
    and     di, 0FFE0h              ; di &= E0h 表示当前条目的起始位置
    add     di, 01Ah                ; di += 1Ah 表示对应条目的簇号的偏移量
    mov     cx, word [es:di]        ; 得到簇号(FAT 序号)
    push    cx                      ; 保存簇号
    add     cx, ax
    add     cx, DeltaSectorNo       ; cx <- LOADER.BIN 的起始扇区号 = 簇号 + RootDirSectors + 19 - 2
    mov     ax, BaseOfLoader
    mov     es, ax                  ; es <- BaseOfLoader
    mov     bx, OffsetOfLoader      ; bx <- OffsetOfLoader
    mov     ax, cx                  ; ax <- LOADER.BIN 的起始扇区号
LABEL_GOON_LOADING_FILE:
    ; 每读取一个扇区就在 “Booting  ”后面追加一个点，形成这样的效果：Booting  .....
    push    ax
    push    bx
    mov     ah, 0Eh
    mov     al, '.'
    mov     bl, 0Fh
    int     10h
    pop     bx
    pop     ax

    mov     cl, 1
    call    ReadSector              ; 根据 AX(起始扇区号) 读取数据区 CL 个扇区数
    pop     ax                      ; 恢复簇号(FAT 序号)
    call    GetFATEntry             ; 得到 FAT 序号
    cmp     ax, 0FFFh               
    jz      LABEL_FILE_LOADED       ; 判断该 FAT 是否为该文件最后一个簇
    push    ax                      ; 保存得到的 FAT
    
    ; 根据 FAT 计算 LOADER.BIN 文件的下一个扇区数据
    mov     dx, RootDirSectors
    add     ax, dx
    add     ax, DeltaSectorNo

    add     bx, [BPB_BytsPerSec]    ; +512偏移量，作为新的 es:bx 读取扇区写入

    jmp     LABEL_GOON_LOADING_FILE
LABEL_FILE_LOADED:
    mov     dh, 1       ; "Ready."
    call    DispStr     ; 显示字符串

;*******************************************************************************
    jmp BaseOfLoader:OffsetOfLoader         ; 跳转到已经加载到内存中的 LOADER.BIN
                                            ; 处开始执行 LOADER.BIN 的代码
                                            ; Boot Sector（引导扇区）的使命到此结束
;*******************************************************************************

;===============================================================================
; 变量
wRootDirSizeForLoop     dw  RootDirSectors  ; Root Directory 占用的扇区数
                                            ; 在循环中会递减置为 0

wSectorNo               dw  0               ; 要读取的扇区号
bOdd                    db  0               ; 奇数 Or 偶数

; 字符串
LoaderFileName          db  "LOADER  BIN", 0 ; LOADER.BIN 之文件名
; 为方便，下面所有字符串长度均为 9
MessageLength   equ 9           ; 统一字符串长度为 9 字节
BootMessage:    db "Booting  "  ; 9 字节，序号 0
Message1        db "Ready.   "  ; 9 字节，序号 1
Message2        db "No LOADER"  ; 9 字节，序号 2
;===============================================================================

;------------------------------------------------------
; 函数名：DispStr
;------------------------------------------------------
; 功能：显示字符串
;------------------------------------------------------
; 参数
;   dh：字符串编号
;------------------------------------------------------
DispStr:
    mov     ax, MessageLength
    mul     dh
    add     ax, BootMessage
    
    ; es:bp = 串地址
    mov     bp, ax
    mov     ax, ds
    mov     es, ax

    mov     cx, MessageLength
    mov     ax, 01301h
    mov     bx, 0007h
    mov     dl, 0
    int     10h

    ret

;------------------------------------------------------
; 函数名：ReadSector
;------------------------------------------------------
; 功能：从第 ax 个扇区开始，将 cl 个扇区读入 es:bx 中
;------------------------------------------------------
; 参数
;   ax：扇区号
;   cl：要读取的扇区数
;------------------------------------------------------
; 返回：es:bx 存放着读取扇区的数据
;------------------------------------------------------
ReadSector:
    push    bp
    mov     bp, sp
    sub     esp, 2 ; 开辟两个字节的堆栈空间，保存要读的扇区数：byte [bp - 2]

    mov     byte[bp - 2], cl    ;
    push    bx                  ; 保存 bx
    mov     bl, [BPB_SecPerTrk] ; bl <- 每磁道扇区数
    div     bl                  ; AL(商-Q), AH(余-R)
    inc     ah                  ; R++
    mov     cl, ah              ; cl <- 起始扇区号
    mov     dh, al              ; dh <- 磁头号
    shr     al, 1               ; 柱面号 = Q >> 1
    mov     ch, al              ; ch <- 柱面号
    and     dh, 1               ; 磁头号 = Q & 1
    pop     bx                  ; 还原 bx
    ; 至此计算完成 柱面号、起始扇区、磁头号
    mov     dl, [BS_DrvNum]     ; 驱动器号（0 表示 A 盘）
.GoOnReading:
    mov ah, 2                   ; 读
    mov al, byte [bp - 2]       ; 读取 al 个扇区，[bp - 2] 就是一开始的 cl
    int 13h
    jc .GoOnReading             ; 若读取错误，则 CF = 1，此时会不停地读，直到正确为止

    add esp, 2
    pop bp

    ret

;------------------------------------------------------
; 函数名：GetFATEntry
;------------------------------------------------------
; 功能：找到序号为 ax 的扇区在 FAT 中的条目，结果放在 ax 中
;       注意：中间需要读 FAT 的扇区到 es:bx 处，所以函数一开始保存了 es 和 bx
;------------------------------------------------------
; 参数
;   AX：簇号
;------------------------------------------------------
GetFATEntry:
    push    es
    push    bx
    push    ax

    ; 在 BaseOfLoader 后面留出 4k 空间用于存放 FAT 表
    mov     ax, BaseOfLoader
    sub     ax, 0100h
    mov     es, ax

    pop     ax
    mov     byte [bOdd], 0      ; 默认簇号为偶数
    mov     bx, 3
    mul     bx                  ; dx:ax = ax * 3
    mov     bx, 2
    div     bx                  ; dx:ax / 2 ==> ax <- 商, dx <- 余数
    cmp     dx, 0
    jz      LABEL_EVEN          ; 判断簇号是否为偶数
    mov     byte [bOdd], 1      ; 不是，是奇数
LABEL_EVEN:
    ; 现在 ax 中是 FATEntry 在 FAT 中的偏移量
    ; 计算 FATEntry 在哪个扇区中（FAT 占用可能不止一个扇区）
    xor     dx, dx
    mov     bx, [BPB_BytsPerSec]
    div     bx ; dx:ax / BPB_BytsPerSec
               ; ax <- 商（FATEntry 所在扇区相对于 FAT 的扇区号）
               ; dx <- 余数（FATEntry 在扇区内的偏移）
    push    dx
    mov     bx, 0 ; bx <- 0 于是，es:bx = (BaseOfLoader - 100):00
    add     ax, SectorNoOfFAT1 ; 此句之后的 ax 就是 FATEntry 所在的扇区号
    mov     cl, 2
    call    ReadSector ; 读取 FATEntry 所在的扇区，一次读两个，避免边界问题，因为一个 FATEntry 可能跨越两个扇区

    pop     dx
    add     bx, dx
    mov     ax, [es:bx]
    cmp     byte [bOdd], 1
    jnz     LABEL_EVEN_2    ; 判断簇号是否为偶数
    shr     ax, 4           ; 不是，是奇数，所以右移 4 位
LABEL_EVEN_2:
    and     ax, 0FFFh       ; 是偶数，去掉高 4 位
LABEL_GET_FAT_ENRY_OK:
    pop     bx
    pop     es

    ret

times 510 - ($ - $$) db 0
dw 0xaa55