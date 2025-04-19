#ifndef _ORANGES_HD_H_
#define _ORANGES_HD_H_

#include "sys/const.h"

/**
 * @struct part_ent
 * @brief  Partition Entry struct.
 *
 * <b>Master Boot Record (MBR):</b>
 *   Located at offset 0x1BE in the 1st sector of a disk. MBR contains
 *   four 16-byte partition entries. Should end with 55h & AAh.
 *
 * <b>partitions in MBR:</b>
 *   A PC hard disk can contain either as many as four primary partitions,
 *   or 1-3 primaries and a single extended partition. Each of these
 *   partitions are described by a 16-byte entry in the Partition Table
 *   which is located in the Master Boot Record.
 *
 * <b>extended partition:</b>
 *   It is essentially a linked list with many tricks. See
 *   http://en.wikipedia.org/wiki/Extended_boot_record for details.
 */
struct part_ent {
    u8  boot_ind;       /**< boot indicator
                          *   Bit 7 is the active partition flag,
                          *   bits 6-0 are zero (when not zero this
                          *   byte is also the drive number of the
                          *   drive to boot so the active partition
                          *   is always found on drive 80H, the first
                          *   hard disk).
                          */

    u8  start_head;     /**< Starting Head */

    u8  start_sector;   /**< Starting Sector.
                          *   Only bits 0-5 are used. Bits 6-7 are
                          *   the upper two bits for the Starting
                          *   Cylinder field.
                          */

    u8  start_cyl;      /**< Starting Cylinder.
                          *   This field contains the lower 8 bits
                          *   of the cylinder value. Starting cylinder
                          *   is thus a 10-bit number, with a maximum
                          *   value of 1023.
                          */

    u8  sys_id;         /**< System ID
                          *   e.g.
                          *   01: FAT12
                          *   81: MINIX
                          *   83: Linux
                          */

    u8  end_head;       /**< Ending Head */

    u8  end_sector;     /**< Ending Sector.
                          *   Only bits 0-5 are used. Bits 6-7 are
                          *   the upper two bits for the Ending
                          *   Cylinder field.
                          */

    u8  end_cyl;        /**< Ending Cylinder.
                          *   This field contains the lower 8 bits
                          *   of the cylinder value. Ending cylinder
                          *   is thus a 10-bit number, with a maximum
                          *   value of 1023.
                          */

    u32 start_sect;     /**< starting sector counting from
                          *   0 / Relative Sector. / start in LBA
                          */

    u32 nr_sects;       /**< number of sectors in partition */
} PARTITION_ENTRY;

/**
 * @struct hd_cmd
 * @brief  Hard drive command structure used for issuing commands to the hard drive.
 */
typedef struct hd_cmd {
    u8  features;    /**< Command features (typically for specific command configurations). */
    u8  count;       /**< Number of sectors for the command operation. */
    u8  lba_low;     /**< Low byte of the Logical Block Address (LBA). */
    u8  lba_mid;     /**< Middle byte of the Logical Block Address (LBA). */
    u8  lba_high;    /**< High byte of the Logical Block Address (LBA). */
    u8  device;      /**< Device to communicate with (typically selects a drive or partition). */
    u8  command;     /**< Command byte (specific command to execute on the device). */
} HD_CMD;

/**
 * @struct part_info
 * @brief  Partition information structure, describing a partition's start sector and size.
 */
typedef struct part_info {
    u32 base;        /**< Starting sector of the partition (NOT byte offset, but sector number). */
    u32 size;        /**< Size of the partition in sectors (NOT byte size, but number of sectors). */
} PART_INFO;

/**
 * @struct hd_info
 * @brief  Main drive information structure, which holds details for a specific hard drive.
 *         One entry is created for each drive.
 */
typedef struct hd_info {
    /* 
    int cylinders;   // Number of cylinders on the drive. (This is typically used for legacy drives.)
    int heads;       // Number of heads per cylinder. (For older drives, it represents physical heads.)
    int sectors;     // Number of sectors per track (track sector count).
    int precomp;     // Cylinder pre-compensation (adjustment based on drive geometry).
    int lzone;       // Landing zone for the heads.
    int ctl;         // Control information specific to the drive.
    */

    int open_cnt;    /**< Open count, tracking how many times this drive has been accessed or opened. */
    
    struct part_info primary[NR_PRIM_PER_DRIVE];   /**< Primary partitions on the drive. 
                                                    *   Array holding information for primary partitions. 
                                                    *   `NR_PRIM_PER_DRIVE` defines the number of primary partitions.
                                                    */
    
    struct part_info logical[NR_SUB_PER_DRIVE];    /**< Logical partitions on the drive.
                                                    *   Array holding information for logical partitions.
                                                    *   `NR_SUB_PER_DRIVE` defines the number of logical partitions.
                                                    */
} HD_INFO;

#endif /* _ORANGES_HD_H_ */