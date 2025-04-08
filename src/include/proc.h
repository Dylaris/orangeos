/*
 * Process
 */

#ifndef _ORANGES_PROC_H
#define _ORANGES_PROC_H

#include "type.h"

#define NR_TASKS 1 /* Number of tasks */
#define STACK_SIZE_TESTA 0x8000 /* stacks of tasks */
#define STACK_SIZE_TOTAL STACK_SIZE_TESTA

typedef struct s_stackframe {
    /* Pushed by kernel.asm:save() */
    u32 gs;
    u32 fs;
    u32 es;
    u32 ds;
    u32 edi;
    u32 dsi;
    u32 ebp;
    u32 kernel_esp;     /* 'popad' will ignore stuff in this position */
    u32 ebx;
    u32 edx;
    u32 ecx;
    u32 eax;

    /* Return address for kernel.asm:save() */
    u32 retaddr;

    /* Pushed by CPU during interrupt */
    u32 eip;
    u32 cs;
    u32 eflags;
    u32 esp;
    u32 ss;
} STACK_FRAME;

typedef struct s_proc {
    STACK_FRAME regs;   /* Process registers saved in stack frame */
    u16 ldt_sel;        /* GDT selector giving LDT base and limit */
    DESCRIPTOR ldts[LDT_SIZE];
} PROCESS;

#endif /* _ORANGES_PROC_H */