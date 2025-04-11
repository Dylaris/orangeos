/*
 * Process
 */

#ifndef _ORANGES_PROC_H_
#define _ORANGES_PROC_H_

#include "type.h"
#include "asm/asm.h"

#define NR_TASKS 4 /* Number of tasks */
#define STACK_SIZE_TTY   0x8000 /* stacks of task tty */
#define STACK_SIZE_TESTA 0x8000 /* stacks of task a */
#define STACK_SIZE_TESTB 0x8000 /* stacks of task b */
#define STACK_SIZE_TESTC 0x8000 /* stacks of task c */
#define STACK_SIZE_TOTAL (STACK_SIZE_TESTA + STACK_SIZE_TESTB + STACK_SIZE_TESTC + STACK_SIZE_TTY)

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

    u16 ldt_sel;        /* LDT selector in GDT */
    DESCRIPTOR ldts[LDT_SIZE];  /* Descriptors in LDT (code and data) */

    int ticks;
    int priority;

    u32 pid;
    char p_name[16];
} PROCESS;

typedef struct s_task {
    task_func initial_eip;
    int stack_size;
    char name[32];
} TASK;

#endif /* _ORANGES_PROC_H_ */