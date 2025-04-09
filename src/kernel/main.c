#include "type.h"
#include "const.h"
#include "proto.h"
#include "global.h"
#include "proc.h"

PUBLIC int kernel_main(void)
{
    disp_str("-----\"kernel_main\" begins-----\n");

    TASK    *p_task       = task_table;
    PROCESS *p_proc       = proc_table;
    char    *p_task_stack = task_stack + STACK_SIZE_TOTAL;
    u16     selector_ldt  = SELECTOR_LDT_FIRST;

    /* Initialize process table */
    for (int i = 0; i < NR_TASKS; i++) {
        strcpy(p_proc->p_name, p_task->name);
        p_proc->pid = i;

        p_proc->ldt_sel = selector_ldt;
        memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(DESCRIPTOR));
        p_proc->ldts[0].attr1 = DA_C | (PRIVILEGE_TASK << 5);
        memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof(DESCRIPTOR));
        p_proc->ldts[1].attr1 = DA_DRW | (PRIVILEGE_TASK << 5);

        p_proc->regs.cs = (0 << 3 & ~SA_RPL_MASK & ~SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.ds = (1 << 3 & ~SA_RPL_MASK & ~SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.es = (1 << 3 & ~SA_RPL_MASK & ~SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.fs = (1 << 3 & ~SA_RPL_MASK & ~SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.ss = (1 << 3 & ~SA_RPL_MASK & ~SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.gs = (SELECTOR_KERNEL_GS & ~SA_RPL_MASK) | RPL_TASK;

        p_proc->regs.eip = (u32) p_task->initial_eip;
        p_proc->regs.esp = (u32) p_task_stack;
        p_proc->regs.eflags = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1 */

        p_task_stack -= p_task->stack_size;
        p_task++;
        p_proc++;
        selector_ldt += 1 << 3;
    }

    /* Initialize 8253 pit (10ms between clock interrupt) */
    out_byte(TIMER_MODE, RATE_GENERATOR);
    out_byte(TIMER0, (u8) (TIMER_FREQ/HZ));
    out_byte(TIMER0, (u8) ((TIMER_FREQ/HZ) >> 8)); 

    /* Set clock interrupt handler */
    put_irq_handler(CLOCK_IRQ, clock_handler);
    enable_irq(CLOCK_IRQ);

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
        disp_str("A.");
        milli_delay(300);
    }
}

void TestB(void)
{
    while (1) {
        disp_str("B.");
        milli_delay(900);
    }
}

void TestC(void)
{
    while (1) {
        disp_str("C.");
        milli_delay(1500);
    }
}