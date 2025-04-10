/*
 * Global variable
 */

#ifndef _ORANGES_SYMBOL_H
#define _ORANGES_SYMBOL_H

#include "type.h"
#include "const.h"
#include "proc.h"
#include "asm/pic.h"
#include "syscall.h"
#include "keyboard.h"

#ifdef SYMBOL_HERE
#undef  EXTERN
#define EXTERN
#endif /* SYMBOL_HERE */

EXTERN int          disp_pos;   /* Display position in screen */
EXTERN int          ticks;      /* Clock ticks */
EXTERN KB_INPUT     kb_in; 

EXTERN char         task_stack[STACK_SIZE_TOTAL];   /* The total stack space for all tasks */
EXTERN TSS          tss;            /* Store the information of running process (ldt and esp0) */
EXTERN PROCESS      *p_proc_ready;  /* Point to the running process */

EXTERN u32          k_reenter;      /* Flag using to diable or enable interrupt reenter */

EXTERN u8           gdt_ptr[6];
EXTERN DESCRIPTOR   gdt[GDT_SIZE];
EXTERN u8           idt_ptr[6];
EXTERN GATE         idt[IDT_SIZE];

EXTERN PROCESS      proc_table[NR_TASKS];   /* Store the context of process (PCB) */
extern TASK         task_table[NR_TASKS];   /* Store the basic information of task, using it to initialize PCB */
EXTERN irq_handler  irq_table[NR_IRQ];      /* Store the irq handler */
extern system_call  sys_call_table[NR_SYS_CALL];

extern u32          keymap[NR_SCAN_CODES * MAP_COLS];

#endif /* _ORANGES_SYMBOL_H */