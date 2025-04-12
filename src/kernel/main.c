#include "const.h"
#include "proc.h"
#include "proto.h"
#include "symbol.h"

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

        p_task_stack -= p_task->stack_size;
        p_task++;
        p_proc++;
        selector_ldt += 1 << 3;
    }

    proc_table[0].ticks = proc_table[0].priority = 15;
    proc_table[1].ticks = proc_table[1].priority = 5;
    proc_table[2].ticks = proc_table[2].priority = 3;

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

void TestA(void)
{
    while (1) {
        // disp_color_str("A.", BRIGHT | MAKE_COLOR(BLACK, RED));
        milli_delay(10);
    }
}

void TestB(void)
{
    while (1) {
        // disp_color_str("B.", BRIGHT | MAKE_COLOR(BLACK, GREEN));
        milli_delay(10);
    }
}

void TestC(void)
{
    while (1) {
        // disp_color_str("C.", BRIGHT | MAKE_COLOR(BLACK, BLUE));
        milli_delay(10);
    }
}