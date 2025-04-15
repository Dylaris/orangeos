#include "sys/const.h"
#include "sys/proc.h"
#include "sys/proto.h"
#include "sys/symbol.h"
#include "type.h"

PUBLIC int kernel_main(void)
{
    disp_str("-----\"kernel_main\" begins-----\n");

    TASK    *p_task       = task_table;
    PROCESS *p_proc       = proc_table;
    char    *p_task_stack = task_stack + STACK_SIZE_TOTAL;
    u16     selector_ldt  = SELECTOR_LDT_FIRST;
    u8      privilege;
    u8      rpl;
    int     eflags;

    /* Initialize process table */
    for (int i = 0; i < NR_TASKS + NR_PROCS; i++) {
        if (i < NR_TASKS) {
            /* Task (ring 1) */
            p_task    = task_table + i;
            privilege = PRIVILEGE_TASK;
            rpl       = RPL_TASK;
            eflags    = 0x1202;     /* IF = 1, IOPL = 1, bit 2 is always 1 */
        } else {
            /* User process (rint 3) */
            p_task    = user_proc_table + (i - NR_TASKS);
            privilege = PRIVILEGE_USER;
            rpl       = RPL_USER;
            eflags    = 0x202;      /* IF = 1, bit 2 is always 1 */
        }

        p_proc->nr_tty = 0;

        strcpy(p_proc->p_name, p_task->name);
        p_proc->pid = i;

        p_proc->ldt_sel = selector_ldt;
        memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(DESCRIPTOR));
        p_proc->ldts[0].attr1 = DA_C | (privilege << 5);
        memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof(DESCRIPTOR));
        p_proc->ldts[1].attr1 = DA_DRW | (privilege << 5);

        p_proc->regs.cs = (0 << 3 & ~SA_RPL_MASK & ~SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.ds = (1 << 3 & ~SA_RPL_MASK & ~SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.es = (1 << 3 & ~SA_RPL_MASK & ~SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.fs = (1 << 3 & ~SA_RPL_MASK & ~SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.ss = (1 << 3 & ~SA_RPL_MASK & ~SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.gs = (SELECTOR_KERNEL_GS & ~SA_RPL_MASK) | rpl;

        p_proc->regs.eip = (u32) p_task->initial_eip;
        p_proc->regs.esp = (u32) p_task_stack;
        p_proc->regs.eflags = eflags;

        p_proc->nr_tty = 0;

        p_task_stack -= p_task->stack_size;
        p_task++;
        p_proc++;
        selector_ldt += 1 << 3;
    }

    proc_table[0].ticks = proc_table[0].priority = 15; /* Necessary for IO responsing */
    proc_table[1].ticks = proc_table[1].priority = 0;
    proc_table[2].ticks = proc_table[2].priority = 0;
    proc_table[3].ticks = proc_table[3].priority = 0;

    proc_table[1].nr_tty = 0;
    proc_table[2].nr_tty = 1;
    proc_table[3].nr_tty = 2;

    /* Reset interrupt handler */
    init_clock();
    init_keyboard();

    /* Initialize global variable */
    ticks = 0;
    k_reenter = 0;
    p_proc_ready = proc_table;

    restart(); /* ring0 to ring1 */

    while (1) {}
}

/**
 * @brief Stop the system when some seriously bad things happen
 * @param fmt Format of rint message
 * @return 
 */
PUBLIC void panic(const char *fmt, ...)
{
    va_list arg = (va_list) ((char *) &fmt + 4);
    char buf[256];
    int len = vsprintf(buf, fmt, arg);
    buf[len] = '\0';
    printl("%c !!panic!! %s", MAG_CH_PANIC, buf);

    /* Should neveer arrive here */
    __asm__ __volatile__ ("ud2");
}

void TestA(void)
{
    while (1) {
        // printf("<Ticks:%x>", get_ticks());
        printf("A.");
        milli_delay(200);
    }
}

void TestB(void)
{
    while (1) {
        printf("B.");
        milli_delay(200);
    }
}

void TestC(void)
{
    while (1) {
        printf("C.");
        milli_delay(200);
    }
}
