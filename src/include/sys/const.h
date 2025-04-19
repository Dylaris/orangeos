/*
 * Common constant
 */

#ifndef _ORANGES_CONST_H_
#define _ORANGES_CONST_H_

#define PUBLIC
#define PRIVATE static
#define EXTERN  extern

#define TRUE    1
#define FALSE   0

#define NULL ((void *) 0)

/*
 * Color
 *
 * e.g. MAKE_COLOR(BLUE, RED)
 *      MAKE_COLOR(BLACK, RED) | BRIGHT
 *      MAKE_COLOR(BLACK, RED) | BRIGHT | FLASH
 */
#define BLACK   0x0     /* 0000 */
#define WHITE   0x7     /* 0111 */
#define RED     0x4     /* 0100 */
#define GREEN   0x2     /* 0010 */
#define BLUE    0x1     /* 0001 */
#define FLASH   0x80    /* 1000 0000 */
#define BRIGHT  0x08    /* 0000 1000 */

#define MAKE_COLOR(b, f) ((b<<4) | f) /* MAKE_COLOR(Background, Foreground) */

#define NR_SYS_CALL 2   /* Number of system call */
#define NR_TASKS    2   /* Number of tasks */
#define NR_PROCS    3   /* Number of processes */
#define NR_CONSOLES 3   /* Number of consoles */

/* Magic chars used by `printx' */
#define MAG_CH_PANIC    '\002'
#define MAG_CH_ASSERT   '\003'

#define ASSERT
#ifdef ASSERT
PUBLIC void assertion_failure(char *exp, char *file, char *base_file, int line);
#define assert(exp) if (exp) ; \
                    else assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__);
#else
#define assert(exp)
#endif /* ASSERT */

#define STR_DEFAULT_LEN 1024

#define DIOCTL_GET_GEO        1

/* Hard Drive */
#define SECTOR_SIZE           512
#define SECTOR_BITS           (SECTOR_SIZE * 8)
#define SECTOR_SIZE_SHIFT     9

/* major device numbers (corresponding to kernel/global.c::dd_map[]) */
#define NO_DEV                0
#define DEV_FLOPPY            1
#define DEV_CDROM             2
#define DEV_HD                3
#define DEV_CHAR_TTY          4
#define DEV_SCSI              5

/* make device number from major and minor numbers */
#define MAJOR_SHIFT           8
#define MAKE_DEV(a, b)        ((a << MAJOR_SHIFT) | b)

/* separate major and minor numbers from device number */
#define MAJOR(x)              ((x >> MAJOR_SHIFT) & 0xFF)
#define MINOR(x)              (x & 0xFF)

/* device numbers of hard disk */
#define MINOR_hd1a            0x10
#define MINOR_hd2a            0x20
#define MINOR_hd2b            0x21
#define MINOR_hd3a            0x30
#define MINOR_hd4a            0x40

#define ROOT_DEV              MAKE_DEV(DEV_HD, MINOR_BOOT)    /* 3, 0x21 */

#define INVALID_INODE         0
#define ROOT_INODE            1

#define MAX_DRIVES            2
#define NR_PART_PER_DRIVE     4
#define NR_SUB_PER_PART       16
#define NR_SUB_PER_DRIVE      (NR_SUB_PER_PART * NR_PART_PER_DRIVE)
#define NR_PRIM_PER_DRIVE     (NR_PART_PER_DRIVE + 1)

/**
 * @def MAX_PRIM_DEV
 * Defines the max minor number of the primary partitions.
 * If there are 2 disks, prim_dev ranges in hd[0-9], this macro will
 * equal 9.
 */
#define MAX_PRIM              (MAX_DRIVES * NR_PRIM_PER_DRIVE - 1)

#define MAX_SUBPARTITIONS     (NR_SUB_PER_DRIVE * MAX_DRIVES)

#define P_PRIMARY             0
#define P_EXTENDED            1

#define ORANGES_PART          0x99    /* Orange'S partition */
#define NO_PART               0x00    /* unused entry */
#define EXT_PART              0x05    /* extended partition */

#define NR_FILES              64
#define NR_FILE_DESC          64  /* FIXME */
#define NR_INODE              64  /* FIXME */
#define NR_SUPER_BLOCK        8

/* INODE::i_mode (octal, lower 32 bits reserved) */
#define I_TYPE_MASK           0170000
#define I_REGULAR             0100000
#define I_BLOCK_SPECIAL       0060000
#define I_DIRECTORY           0040000
#define I_CHAR_SPECIAL        0020000
#define I_NAMED_PIPE          0010000

#define is_special(m)         ((((m) & I_TYPE_MASK) == I_BLOCK_SPECIAL) ||   \
                                (((m) & I_TYPE_MASK) == I_CHAR_SPECIAL))

#define NR_DEFAULT_FILE_SECTS 2048   /* 2048 * 512 = 1MB */

#endif /* _ORANGES_CONST_H_ */