#ifndef _ORANGES_ASM_HD_H_
#define _ORANGES_ASM_HD_H_

/********************************************/
/* I/O Ports used by hard disk controllers. */
/********************************************/
/* slave disk not supported yet, all master registers below */

/* Command Block Registers */
/*	MACRO		PORT			DESCRIPTION			INPUT/OUTPUT	*/
/*	-----		----			-----------			------------	*/
#define REG_DATA         0x1F0    /* Data                  I/O    */
#define REG_FEATURES     0x1F1    /* Features              O      */
#define REG_ERROR        REG_FEATURES  /* Error               I    */
                    /* The contents of this register are valid only when the error bit
                       (ERR) in the Status Register is set, except at drive power-up or at the
                       completion of the drive's internal diagnostics, when the register
                       contains a status code.
                       When the error bit (ERR) is set, Error Register bits are interpreted as such:
                       |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
                       +-----+-----+-----+-----+-----+-----+-----+-----+
                       | BRK | UNC |     | IDNF|     | ABRT|TKONF| AMNF|
                       +-----+-----+-----+-----+-----+-----+-----+-----+
                       - BRK: Bad Block Mark
                       - UNC: Uncorrectable Data Error
                       - IDNF: ID Not Found
                       - ABRT: Command Aborted
                       - TKONF: Track 0 Not Found
                       - AMNF: Address Mark Not Found
                    */
#define REG_NSECTOR      0x1F2    /* Sector Count          I/O    */
#define REG_LBA_LOW      0x1F3    /* Sector Number / LBA Bits 0-7   I/O */
#define REG_LBA_MID      0x1F4    /* Cylinder Low / LBA Bits 8-15   I/O */
#define REG_LBA_HIGH     0x1F5    /* Cylinder High / LBA Bits 16-23  I/O */
#define REG_DEVICE       0x1F6    /* Drive | Head | LBA bits 24-27   I/O */
                    /* |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
                       +-----+-----+-----+-----+-----+-----+-----+-----+
                       |  1  |  L  |  1  | DRV | HS3 | HS2 | HS1 | HS0 |
                       +-----+-----+-----+-----+-----+-----+-----+-----+
                         - L: LBA Mode. If L=1, the register is in LBA mode; otherwise, in CHS mode.
                         - DRV: Drive selection (0 for master, 1 for slave)
                         - HS3/HS2/HS1/HS0: Head Select bits or LBA bits depending on mode
                    */
#define REG_STATUS       0x1F7    /* Status                I      */
                    /* Any pending interrupt is cleared whenever this register is read.
                       |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
                       +-----+-----+-----+-----+-----+-----+-----+-----+
                       | BSY | DRDY|DF/SE|  #  | DRQ |     |     | ERR |
                       +-----+-----+-----+-----+-----+-----+-----+-----+
                       - BSY: Busy (1 when the drive is busy)
                       - DRDY: Drive Ready (1 when the drive is ready)
                       - DF/SE: Device Fault / Stream Error (1 if there's an error)
                       - DRQ: Data Request (1 when data is ready to be transferred)
                       - ERR: Error (1 if an error occurred)
                    */
#define STATUS_BSY       0x80
#define STATUS_DRDY      0x40
#define STATUS_DFSE      0x20
#define STATUS_DRQ       0x08
#define STATUS_ERR       0x01

#define REG_CMD          REG_STATUS /* Command              O      */

/* Control Block Registers */
/*	MACRO		PORT			DESCRIPTION			INPUT/OUTPUT	*/
/*	-----		----			-----------			------------	*/
#define REG_DEV_CTRL     0x3F6    /* Device Control        O      */
                    /* |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
                       +-----+-----+-----+-----+-----+-----+-----+-----+
                       | HOB |  -  |  -  |  -  |  -  |SRST |-IEN |  0  |
                       +-----+-----+-----+-----+-----+-----+-----+-----+
                         - HOB: High Order Byte for 48-bit addressing.
                         - SRST: Software Reset (1 resets the device).
                         - IEN: Interrupt Enable (1 disables interrupts).
                    */
#define REG_ALT_STATUS   REG_DEV_CTRL /* Alternate Status    I      */
                    /* This register contains the same information as the Status Register.
                       The only difference is that reading this register does not imply interrupt acknowledge or clear a pending interrupt. */

#define REG_DRV_ADDR     0x3F7    /* Drive Address          I     */

/***************/
/* DEFINITIONS */
/***************/
#define HD_TIMEOUT               10000  /* in millisec */
#define PARTITION_TABLE_OFFSET    0x1BE
#define ATA_IDENTIFY              0xEC
#define ATA_READ                  0x20
#define ATA_WRITE                 0x30

/* for DEVICE register. */
#define MAKE_DEVICE_REG(lba, drv, lba_highest)  \
    (((lba) << 6) |                            \
     ((drv) << 4) |                            \
     ((lba_highest) & 0xF) | 0xA0)

#endif /* _ORANGES_ASM_HD_H_ */