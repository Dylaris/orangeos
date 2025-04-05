org 0100h

    jmp     LABEL_START

%include    "include/fat12hdr.inc"
%include    "include/pm.inc"
%include    "include/load.inc"

; =================================================================
; GDT
;                           段地址        段界限   属性
LABEL_GDT:          Descriptor 0,             0, 0
LABEL_DESC_FLAT_C:  Descriptor 0,       0fffffh, DA_CR | DA_32 | DA_LIMIT_4K 	; 0-4GB可执行段
LABEL_DESC_FLAT_RW: Descriptor 0,       0fffffh, DA_DRW | DA_32 | DA_LIMIT_4K 	; 0-4GB可读写段
LABEL_DESC_VIDEO:   Descriptor 0B8000h,  0ffffh, DA_DRW | DA_DPL3

GdtLen      equ     $ - LABEL_GDT                   ; GDT 长度
GdtPtr      dw      GdtLen - 1                      ; 段界限
            dd      BaseOfLoaderPhyAddr + LABEL_GDT ; 基地址

; GDT 选择子
SelectorFlatC       equ     LABEL_DESC_FLAT_C   -   LABEL_GDT
SelectorFlatRW      equ     LABEL_DESC_FLAT_RW  -   LABEL_GDT
SelectorVideo       equ     LABEL_DESC_VIDEO    -   LABEL_GDT + SA_RPL3

; 栈基地址
BaseOfStack         equ     0100h
; =================================================================

LABEL_START:
    mov     ax, cs
    mov     ds, ax
    mov     es, ax
    mov     ss, ax
    mov     sp, BaseOfStack

    mov     dh, 0       ; "Looting  "
    call    DispStrRealMode     ; 显示字符串

    ; 得到内存数
    mov     ebx, 0
    mov     di, _MemChkBuf
.MemChkLoop:
    mov     eax, 0E820h
    mov     ecx, 20
    mov     edx, 0534D4150h
    int     15h
    jc      .MemChkFail
    add     di, 20
    inc     dword [_dwMCRNumber]
    cmp     ebx, 0
    jne     .MemChkLoop
    jmp     .MemChkOK
.MemChkFail:
    mov     dword [_dwMCRNumber], 0
.MemChkOK:

    xor     ah, ah  
    xor     dl, dl  ; 软驱复位
    int     13h      

; 在 A 盘的根目录寻找 KERNEL.BIN
    mov     word [wSectorNo], SectorNoOfRootDirectory
LABEL_SEARCH_IN_ROOT_DIR_BEGIN:
    cmp     word [wRootDirSizeForLoop], 0   ; 判断根目录区是否已读完
    jz      LABEL_NO_KERNELBIN              ; 若读完，则表示没有找到 KERNEL.BIN
    dec     word [wRootDirSizeForLoop]      ; 根目录占用的扇区数 -1
    mov     ax, BaseOfKernelFile
    mov     es, ax                          ; es <- BaseOfKernelFile
    mov     bx, OffsetOfKernelFile              ; bx <- OffsetOfKernelFile
    mov     ax, [wSectorNo]                 ; ax <- Root Director 中的某 Sector 号
    mov     cl, 1
    call    ReadSector                      ; 执行后，加载的结束根目录区中第 wSectorNo 号扇区的数据（加载位置：es:bx）

    mov     si, KernelFileName              ; ds:si -> "KERNEL  BIN"
    mov     di, OffsetOfKernelFile              ; es:di -> BaseOfKernelFile:0100
    cld
    mov     dx, 10h                         ; 根目录每个扇区的最大文件数(条目) = 512(一个扇区大小) / 32(条目大小) = 16 = 10H
LABEL_SEARCH_FOR_KERNELBIN:                 ; 遍历扇区中的每个条目
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
    mov     si, KernelFileName
    jmp     LABEL_SEARCH_FOR_KERNELBIN

LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR:         ; 在根目录区内跳转到下一个扇区
    add     word [wSectorNo], 1             ; 要读取的扇区号 +1
    jmp     LABEL_SEARCH_IN_ROOT_DIR_BEGIN  ; 重新去读取这个扇区

LABEL_NO_KERNELBIN:     ; 找不到 KERNEL.BIN
    mov     dh, 2       ; 字符串：No KERNEL.
    call DispStrRealMode        ; 显示字符串

%ifdef _BOOT_DEBUG_
    mov     ax, 4c00h
    int     21h         ; DEBUG 状态下，没有找到 KERNEL.BIN 则回到 DOS
%else
    jmp     $           ; 没有找到 KERNEL.BIN 则死循环在此处
%endif

LABEL_FILENAME_FOUND:   ; 找到 KERNEL.BIN 后，便来到这里。
    mov     ax, RootDirSectors
    and     di, 0FFE0h              ; di &= E0h 表示当前条目的起始位置

    push    eax                         ;| 
    mov     eax, [es:di + 01Ch]         ;| // 这些都是新增加的
    mov     dword [dwKernelSize], eax   ;| // 保存 KERNEL.BIN 的文件大小
    pop     eax                         ;| 

    add     di, 01Ah                ; di += 1Ah 表示对应条目的簇号的偏移量
    mov     cx, word [es:di]        ; 得到簇号(FAT 序号)
    push    cx                      ; 保存簇号
    add     cx, ax
    add     cx, DeltaSectorNo       ; cx <- KERNEL.BIN 的起始扇区号 = 簇号 + RootDirSectors + 19 - 2
    mov     ax, BaseOfKernelFile
    mov     es, ax                  ; es <- BaseOfKernelFile
    mov     bx, OffsetOfKernelFile  ; bx <- OffsetOfKernelFile
    mov     ax, cx                  ; ax <- KERNEL.BIN 的起始扇区号
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
    
    ; 根据 FAT 计算 KERNEL.BIN 文件的下一个扇区数据
    mov     dx, RootDirSectors
    add     ax, dx
    add     ax, DeltaSectorNo

    add     bx, [BPB_BytsPerSec]    ; +512偏移量，作为新的 es:bx 读取扇区写入

    jmp     LABEL_GOON_LOADING_FILE
LABEL_FILE_LOADED:
    call    KillMotor   ; 关闭软驱马达

    mov     dh, 1       ; "Ready."
    call    DispStrRealMode     ; 显示字符串

    ; 下面准备跳入保护模式
    
    lgdt    [GdtPtr]    ; 加载 GDTR

    cli                 ; 关中断

    ; 打开地址线 A20
    in      al, 92h
    or      al, 00000010b
    out     92h, al

    ; 开启保护模式的开关
    mov     eax, cr0
    or      eax, 1
    mov     cr0, eax

    ; 跳入保护模式
    jmp     dword SelectorFlatC:(BaseOfLoaderPhyAddr + LABEL_PM_START)

;*******************************************************************************
    jmp BaseOfKernelFile:OffsetOfKernelFile         ; 跳转到已经加载到内存中的 KERNEL.BIN
                                            ; 处开始执行 KERNEL.BIN 的代码
                                            ; Boot Sector（引导扇区）的使命到此结束
;*******************************************************************************

;===============================================================================
; 变量
;-------------------------------------------------------------------------------
wRootDirSizeForLoop     dw  RootDirSectors  ; Root Directory 占用的扇区数
                                            ; 在循环中会递减置为 0

wSectorNo               dw  0               ; 要读取的扇区号
bOdd                    db  0               ; 奇数 Or 偶数
dwKernelSize            dd  0               ; KERNEL.BIN 文件大小

;===============================================================================
; 字符串
;-------------------------------------------------------------------------------
KernelFileName          db  "KERNEL  BIN", 0 ; KERNEL.BIN 之文件名
; 为方便，下面所有字符串长度均为 9
MessageLength   equ 9           ; 统一字符串长度为 9 字节
LoadMessage:    db "Looting  "  ; 9 字节，序号 0
Message1        db "Ready.   "  ; 9 字节，序号 1
Message2        db "No KERNEL"  ; 9 字节，序号 2
;===============================================================================

;------------------------------------------------------
; 函数名：DispStr
;------------------------------------------------------
; 功能：显示字符串
;------------------------------------------------------
; 参数
;   dh：字符串编号
;------------------------------------------------------
DispStrRealMode:
    mov     ax, MessageLength
    mul     dh
    add     ax, LoadMessage
    
    ; es:bp = 串地址
    mov     bp, ax
    mov     ax, ds
    mov     es, ax

    mov     cx, MessageLength   ; cx = 串长度
    mov     ax, 01301h          ; AH = 13, AL = 01h
    mov     bx, 0007h           ; 页号为 0(BH = 0) 黑底白字(BL = 07h)
    mov     dl, 0
    add     dh, 3 ; 从第 3 行开始往下显示
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

    ; 在 BaseOfKernelFile 后面留出 4k 空间用于存放 FAT 表
    mov     ax, BaseOfKernelFile
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

;------------------------------------------------------
; 函数名：KillMotor
;------------------------------------------------------
; 功能：关闭软驱马达，否则软驱灯会一直亮着。
;------------------------------------------------------
KillMotor:
    push    dx
    mov     dx, 03F2h
    mov     al, 0
    out     dx, al
    pop     dx
    ret

; ==================================================================================
; 保护模式
; 32 位代码段，由实模式跳入
[SECTION .s32]

ALIGN   32

[BITS   32]

LABEL_PM_START:
    mov     ax, SelectorVideo
    mov     gs, ax

    ; 初始化各个寄存器
    mov     ax, SelectorFlatRW
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     ss, ax
    mov     esp, TopOfStack

    push    szMemChkTitle
    call    DispStr
    add     esp, 4

    call    DispMemInfo ; 打印内存信息
    call    SetupPaging ; 开启分页机制

    call    InitKernel

    ;**************************************************************
    jmp     SelectorFlatC:KernelEntryPointPhyAddr   ; 正式进入内核
    ;**************************************************************

%include "include/lib.inc"

;------------------------------------------------------
; 函数名：DispMemInfo
;------------------------------------------------------
; 功能：读取内存信息
;------------------------------------------------------
DispMemInfo:
    push    esi
    push    edi
    push    ecx

    mov     esi, MemChkBuf
    mov     ecx, [dwMCRNumber]
.loop:
    mov     edx, 5
    mov     edi, ARDStruct
.1:
    push    dword [esi]
    call    DispInt
    pop     eax
    stosd
    add     esi, 4
    dec     edx
    cmp     edx, 0
    jnz     .1
    call    DispReturn
    cmp     dword [dwType], 1
    jne     .2
    mov     eax, [dwBaseAddrLow]
    add     eax, [dwLengthLow]
    cmp     eax, [dwMemSize]
    jb      .2
    mov     [dwMemSize], eax
.2:
    loop    .loop

    call    DispReturn
    push    szRAMSize
    call    DispStr
    add     esp, 4

    push    dword [dwMemSize]
    call    DispInt
    add     esp, 4

    pop     ecx
    pop     edi
    pop     esi

    ret

;---------------------------------------------------------------
; 函数名：SetupPaging
;---------------------------------------------------------------
; 功能：启动分页机制
;---------------------------------------------------------------
SetupPaging:
    ; 根据内存大小计算应初始化多少 PDE 以及多少页表
    xor     edx, edx
    mov     eax, [dwMemSize]
    mov     ebx, 400000h        ; 400000h = 4M = 4096 * 1024, 一个页表对应的内存大小
    div     ebx
    mov     ecx, eax            ; 此时 ecx 为页表的个数，也即 PDE 应该的个数
    test    edx, edx
    jz      .no_remainder
    inc     ecx                 ; 若余数不为 0，就增加一个页表
.no_remainder:
    push    ecx                 ; 暂存页表个数

    ; 为简化处理，所有线性地址对应相等的物理地址，并且不考虑内存空洞

    ; 首先 初始化页目录
    mov     ax, SelectorFlatRW
    mov     es, ax
    mov     edi, PageDirBase
    xor     eax, eax
    mov     eax, PageTblBase | PG_P | PG_USU | PG_RWW ; 构建 PDE
.1:
    stosd
    add     eax, 4096
    loop    .1

    ; 再初始化所有页表
    pop     eax                 ; 页表个数
    mov     ebx, 1024           ; 每个页表 1024 个 PTE
    mul     ebx
    mov     ecx, eax            ; PTE 个数 = 页表 * 1024
    mov     edi, PageTblBase    ; 此段首地址为 PageTblBase
    xor     eax, eax
    mov     eax, PG_P | PG_USU | PG_RWW ; 构建 PTE
.2:
    stosd
    add     eax, 4096           ; 每一页都指向 4K 的空间
    loop    .2

    mov     eax, PageDirBase
    mov     cr3, eax            ; 将页目录的物理地址保存到 CR3 寄存器中
    mov     eax, cr0
    or      eax, 80000000h      ; 设置 CR0 的最高 PG 位为 1，开启分页机制
    mov     cr0, eax
    jmp     short .3
.3:
    nop

    ret

;---------------------------------------------------------------
; 函数名：InitKernel
;---------------------------------------------------------------
; 功能：将 KERNEL.BIN 的内容经过整理对其后放到新的位置
; 遍历每个 Program Header，根据 Program Header 中的信息
; 来确定把什么放进内存，放到什么位置，以及放多少。
;---------------------------------------------------------------
InitKernel:
    xor     esi, esi
    mov     cx, word [BaseOfKernelFilePhyAddr + 2Ch]
    movzx   ecx, cx
    mov     esi, [BaseOfKernelFilePhyAddr + 1Ch]
    add     esi, BaseOfKernelFilePhyAddr
.Begin:
    mov     eax, [esi + 0]
    cmp     eax, 0
    jz      .NoAction
    push    dword [esi + 010h]
    mov     eax, [esi + 04h]
    add     eax, BaseOfKernelFilePhyAddr
    push    eax
    push    dword [esi + 08h]
    call    MemCpy
    add     esp, 12
.NoAction:
    add     esi, 020h
    dec     ecx
    jnz     .Begin

    ret

; ----------------------------------------------------------------------------
; 数据段
[SECTION .data1]

ALIGN   32

LABEL_DATA:
; 实模式下使用这些符号
; 字符串
_szMemChkTitle:	db "BaseAddrL BaseAddrH LengthLow LengthHigh   Type", 0Ah, 0
_szRAMSize:	    db "RAM size:", 0
_szReturn:	    db 0Ah, 0
;; 变量
_dwMCRNumber:	dd 0	; Memory Check Result
_dwDispPos:	    dd (80 * 6 + 0) * 2	; 屏幕第 6 行, 第 0 列
_dwMemSize:	    dd 0
_ARDStruct:	; Address Range Descriptor Structure
  _dwBaseAddrLow:		dd	0
  _dwBaseAddrHigh:		dd	0
  _dwLengthLow:			dd	0
  _dwLengthHigh:		dd	0
  _dwType:	    		dd	0
_MemChkBuf:	times	256	db	0
;
;; 保护模式下使用这些符号
szMemChkTitle		equ	BaseOfLoaderPhyAddr + _szMemChkTitle
szRAMSize	    	equ	BaseOfLoaderPhyAddr + _szRAMSize
szReturn	    	equ	BaseOfLoaderPhyAddr + _szReturn
dwDispPos	    	equ	BaseOfLoaderPhyAddr + _dwDispPos
dwMemSize	    	equ	BaseOfLoaderPhyAddr + _dwMemSize
dwMCRNumber	    	equ	BaseOfLoaderPhyAddr + _dwMCRNumber
ARDStruct	    	equ	BaseOfLoaderPhyAddr + _ARDStruct
	dwBaseAddrLow	equ	BaseOfLoaderPhyAddr + _dwBaseAddrLow
	dwBaseAddrHigh	equ	BaseOfLoaderPhyAddr + _dwBaseAddrHigh
	dwLengthLow	    equ	BaseOfLoaderPhyAddr + _dwLengthLow
	dwLengthHigh	equ	BaseOfLoaderPhyAddr + _dwLengthHigh
	dwType	    	equ	BaseOfLoaderPhyAddr + _dwType
MemChkBuf	    	equ	BaseOfLoaderPhyAddr + _MemChkBuf

StackSpace: times   1024    db 0
TopOfStack  equ     BaseOfLoaderPhyAddr + $ ; 栈顶

