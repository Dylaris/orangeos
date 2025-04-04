; FAT12 磁盘的头
; --------------------------------------------------------------------------
BS_OEMName      DB 'ForrestY'   ; OEM String, 必须 8 个字节
BPB_BytsPerSec  DW 512          ; 每扇区字节数
BPB_SecPerClus  DB 1            ; 没簇多少扇区
BPB_RsvdSecCnt  DW 1            ; Boot 记录多少扇区
BPB_NumFATs     DB 2            ; 共有多少 FAT 表
BPB_RootEntCnt  DW 224          ; 根目录文件数最大值
BPB_TotSec16    DW 2880         ; 逻辑扇区总数
BPB_Media       DB 0xF0         ; 媒体描述符
BPB_FATSz16     DW 9            ; 每个 FAT 扇区数
BPB_SecPerTrk   DW 18           ; 每磁道扇区数
BPB_NumHeads    DW 2            ; 磁头数（面数）
BPB_HiddSec     DD 0            ; 隐藏扇区数
BPB_TotSec32    DD 0            ; wTotalSectorCount 为 0 时这个值记录扇区数
BS_DrvNum       DB 0            ; 中断 13 的驱动器号
BS_Reserved1    DB 0            ; 未使用
BS_BootSig      DB 29h          ; 扩展引导标记（29h）
BS_VolID        DD 0            ; 卷序列号
BS_VolLab       DB 'OrangeS0.02'; 卷标，必须 11 个字节
BS_FileSysType  DB 'FAT12   '   ; 文件系统类型，必须 8 个字节
; --------------------------------------------------------------------------

; --------------------------------------------------------------------------
; 基于 FAT12 头的一些常量定义，若头信息改变，下面的常量可能也要做相应的改变
; --------------------------------------------------------------------------
FATSz           equ 9       ; BPB_FATSz16

; RootDirSectors = ((BPB_RootEntCnt * 2) + (BPB_BytsPerSec - 1)) / BPB_BytsPerSec
RootDirSectors  equ 14      ; 根目录占用空间

SectorNoOfRootDirectory equ 19  ; Root Director 的第一个扇区号

SectorNoOfFAT1  equ 1       ; FAT1 的第一个扇区号 = BPB_RsvdSecCnt

DeltaSectorNo   equ 17      ; DeltaSectorNo = BPB_RsvdSecCnt + (BPB_NumFATs * FATSz) - 2
                            ; 文件的起始扇区号 = DirEntry 中的起始扇区号 + 根目录占用的扇区数 + DeltaSectorNo



