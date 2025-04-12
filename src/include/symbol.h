/*
 * Global variable
 */

#ifndef _ORANGES_SYMBOL_H_
#define _ORANGES_SYMBOL_H_

#include "proc.h"
#include "syscall.h"
#include "keyboard.h"
#include "tty.h"
#include "console.h"

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

EXTERN int          nr_current_console;

EXTERN u8           gdt_ptr[6];
EXTERN DESCRIPTOR   gdt[GDT_SIZE];
EXTERN u8           idt_ptr[6];
EXTERN GATE         idt[IDT_SIZE];

extern PROCESS      proc_table[];       /* Store the context of all processes (PCB) */
extern TASK         task_table[];       /* Ring 1 process (task) */
extern TASK         user_proc_table[];  /* Ring 3 process (user process) */
extern irq_handler  irq_table[];        /* Store the irq handler */
extern system_call  sys_call_table[];   /* Used in kernel space */
extern TTY          tty_table[];        /* TTY table (keyboard and console) */
extern CONSOLE      console_table[];    /* Console table (console write/read and cursor) */
extern u32          keymap[];

#endif /* _ORANGES_SYMBOL_H_ */