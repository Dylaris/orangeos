#include "asm/pic.h"
#include "asm/hd.h"
#include "asm/pit.h"
#include "sys/const.h"
#include "sys/proc.h"
#include "sys/proto.h"
#include "sys/hd.h"
#include "type.h"

PRIVATE void init_hd(void);
PRIVATE void hd_cmd_out(HD_CMD *cmd);
PRIVATE int  waitfor(int mask, int val, int timeout);
PRIVATE void interrupt_wait(void);
PRIVATE void hd_identify(int drive);
PRIVATE void print_identify_info(u16 *hdinfo);

PRIVATE u8 hd_status;
PRIVATE u8 hdbuf[SECTOR_SIZE * 2];

/**
 * @brief Main loop of hard driver
 */
PUBLIC void task_hd(void)
{
    init_hd();

    MESSAGE msg;
    while (1) {
        send_recv(RECEIVE, ANY, &msg);
        int src = msg.source;

        switch (msg.type) {
        case DEV_OPEN:
            hd_identify(0);
            break;

        default:
            spin("FS::main_loop (invalid msg.type)");
            break;
        }

        send_recv(SEND, src, &msg);
    }
}

/**
 * @brief <Ring 1> Check hard drive, set IRQ handler, enable IRQ and initialize data
 */
PRIVATE void init_hd(void)
{
    /* Get the number of drives from the BIOS data area */
    u8 *p_nr_drives = (u8 *) 0x475;
    printl("NrDrives:%d.\n", *p_nr_drives);
    assert(*p_nr_drives);

    put_irq_handler(AT_WINI_IRQ, hd_handler);
    enable_irq(CASCADE_IRQ);
    enable_irq(AT_WINI_IRQ);
}

/**
 * @brief <Ring 1> Get the disk information
 * @param drive Drive Number
 */
PRIVATE void hd_identify(int drive)
{
    HD_CMD cmd;
    cmd.device = MAKE_DEVICE_REG(0, drive, 0);
    cmd.command = ATA_IDENTIFY;
    hd_cmd_out(&cmd);
    interrupt_wait();
    port_read(REG_DATA, hdbuf, SECTOR_SIZE);

    print_identify_info((u16 *) hdbuf);
}

/**
 * @brief <Ring 1> Output a command to HD controller
 * @param cmd The command struct ptr
 */
PRIVATE void hd_cmd_out(HD_CMD *cmd)
{
    /* For all commands, the host must first check if BSY=1,
       and should proceed no further unless and until BSY=0 */
    if (!waitfor(STATUS_BSY, 0, HD_TIMEOUT)) panic("hd error.");

    /* Activate the Interrupt Enable (nIEN) bit */
    out_byte(REG_DEV_CTRL, 0);
    /* Load required parameters in the Command Block Registers */
    out_byte(REG_FEATURES, cmd->features);
    out_byte(REG_NSECTOR, cmd->count);
    out_byte(REG_LBA_LOW, cmd->lba_low);
    out_byte(REG_LBA_MID, cmd->lba_mid);
    out_byte(REG_LBA_HIGH, cmd->lba_high);
    out_byte(REG_DEVICE, cmd->device);
    /* Write the command code to the Command Register */
    out_byte(REG_CMD, cmd->command);
}

/**
 * @brief <Ring 1> Wait for a certain status
 * @param mask    Status mask
 * @param val     Required status
 * @param timeout Timeout in milliseconds
 * @return One if success, zero if timeout
 */
PRIVATE int waitfor(int mask, int val, int timeout)
{
    int t = get_ticks();

    while (((get_ticks() - t) * 1000 / HZ) < timeout)
        if ((in_byte(REG_STATUS) & mask) == val)
            return 1;

    return 0;
}

PRIVATE void interrupt_wait(void)
{
    MESSAGE msg;
    send_recv(RECEIVE, INTERRUPT, &msg); 
}

/**
 * @brief <Ring 1> Print the hdinfo retrieved via ATA_IDENTIFY command.
 * @param hdinfo The buffer read from the disk I/O port
 */
PRIVATE void print_identify_info(u16 *hdinfo)
{
    struct iden_info_ascii {
        int idx;
        int len;
        char *desc;
    } info[] = {
        {10, 20, "HD SN"},      /* Serial numer in ASCII */
        {27, 40, "HD Model"}    /* Model number in ASCII */
    };

    char buf[64];

    for (int i = 0; i < sizeof(info)/sizeof(info[0]); i++) {
        char *p = (char *) &hdinfo[info[i].idx];
        for (int j = 0; j < info[i].len/2; j++) {
            buf[i*2+1] = *p++;
            buf[i*2] = *p++;
        }
        buf[i*2] = '\0';
        printl("%s: %s\n", info[i].desc, buf);
    }

    int capabilities = hdinfo[49];
    printl("LBA supported: %s\n", (capabilities & 0x0200) ? "Yes" : "No");

    int cmd_set_supported = hdinfo[83];
    printl("LBA supported: %s\n", (cmd_set_supported & 0x0400) ? "Yes" : "No");

    int sectors = ((int) hdinfo[61] << 16) + hdinfo[60];
    printl("HD size: %dMB\n", sectors * 512 / 1000000);
}

/**
 * @brief <Ring 0> Interrupt handler
 * @param irq IRQ nr of the disk interrupt
 */
PUBLIC void hd_handler(int irq)
{
    /* Interrupts are cleared when the host 
        - reads the Status Register 
        - issues a rest, or
        - writes to the Command Register */
    hd_status = in_byte(REG_STATUS);

    inform_int(TASK_HD);
}
