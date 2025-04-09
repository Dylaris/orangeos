/*
 * Global variable
 */

#ifndef _ORANGES_GLOBAL_H
#define _ORANGES_GLOBAL_H

#include "type.h"
#include "const.h"
#include "proc.h"

#ifdef GLOBAL_VARIABLE_HERE
#undef  EXTERN
#define EXTERN
#endif /* GLOBAL_VARIABLE_HERE */

EXTERN int          disp_pos;   /* Display position in screen */

EXTERN u8           gdt_ptr[6];
EXTERN DESCRIPTOR   gdt[GDT_SIZE];
EXTERN u8           idt_ptr[6];
EXTERN GATE         idt[IDT_SIZE];

EXTERN PROCESS      proc_table[NR_TASKS];           /* Store the context of process (PCB) */
EXTERN TASK         task_table[NR_TASKS];           /* Store the basic information of task, using it to initialize PCB */
EXTERN char         task_stack[STACK_SIZE_TOTAL];   /* The total stack space for all tasks */
EXTERN TSS          tss;
EXTERN PROCESS      *p_proc_ready;  /* Point to the running process */

EXTERN u32          k_reenter;      /* Flag using to diable or enable interrupt reenter */

#endif /* _ORANGES_GLOBAL_H */