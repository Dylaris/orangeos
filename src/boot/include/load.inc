BaseOfLoader        equ     09000h  ; LOADER.BIN 被加载到的位置 —— 段地址
OffsetOfLoader      equ     0100h   ; LOADER.BIN 被加载到的位置 —— 偏移地址

BaseOfLoaderPhyAddr equ     BaseOfLoader * 10h  ; LOADER.BIN 被加载到的位置 —— 物理地址

BaseOfKernelFile    equ     08000h  ; KERNEL.BIN 被加载到的位置 —— 段地址
OffsetOfKernelFile  equ     0h      ; KERNEL.BIN 被加载到的位置 —— 偏移地址

BaseOfKernelFilePhyAddr     equ     BaseOfKernelFile * 10h ; KERNEL.BIN 倍加载到的位置 —— 物理地址
KernelEntryPointPhyAddr     equ     30400h  ; 内核的入口偏移地址
                                            ; 必须和 -Text 的值相等
; 分页常量
PageDirBase     equ     200000h     ; 页目录起始地址: 2M
PageTblBase     equ     201000h     ; 页表起始地址: 2M + 4K

