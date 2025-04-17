/*
 * Process
 */

#ifndef _ORANGES_PROC_H_
#define _ORANGES_PROC_H_

#include "asm/asm.h"
#include "type.h"

/* Tasks */
#define INVALID_DRIVER  -20
#define INTERRUPT       -10
#define TASK_TTY        0
#define TASK_SYS        1
#define TASK_WINCH      2 
#define TASK_FS         3
#define TASK_MM         4
#define ANY             (NR_TASKS + NR_PROCS + 10)
#define NO_TASK         (NR_TASKS + NR_PROCS + 20)

#define FIRST_PROC  proc_table[0]
#define LAST_PROC   proc_table[NR_TASKS + NR_PROCS - 1]

#define STACK_SIZE_TTY   0x8000 /* stacks of task tty */
#define STACK_SIZE_SYS   0x8000 /* stacks of task sys */
#define STACK_SIZE_TESTA 0x8000 /* stacks of task a */
#define STACK_SIZE_TESTB 0x8000 /* stacks of task b */
#define STACK_SIZE_TESTC 0x8000 /* stacks of task c */
#define STACK_SIZE_TOTAL (STACK_SIZE_TTY + \
                          STACK_SIZE_SYS + \
                          STACK_SIZE_TESTA + \
                          STACK_SIZE_TESTB + \
                          STACK_SIZE_TESTC)

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

    int ticks;          /* Remained execution time */
    int priority;       /* For process schedule */

    u32 pid;
    char p_name[16];

    int p_flags;        /* 
                         * Process flags 
                         * RUNNING / SENDING / RECEIVING
                         */
    MESSAGE *p_msg;     /* Point to message body */
    int p_recvfrom;     /* Record from whom process wants to receive a message */
    int p_sendto;       /* Record to whom process wants to send a message */

    int has_int_msg;    /* It will be set one when the interrupt which process is waiting for occurs */

    struct s_proc *q_sending;       /* Queue of processes sending messages to this proc */
    struct s_proc *next_sending;    /* Next processes in the sending queue */

    int nr_tty;         /* Control tty */
} PROCESS;

typedef struct s_task {
    task_func initial_eip;
    int stack_size;
    char name[32];
} TASK;

#define proc2pid(x) ((x) - proc_table)

/* Process p_flags */
#define RUNNING     0
#define SENDING     (1 << 1)    /* Set when process try to send */
#define RECEIVING   (1 << 2)    /* Set when process try to receive */

/* IPC */
#define SEND    1
#define RECEIVE 2
#define BOTH    3 /* BOTH = (SEND | RECEIVE) */

/**
 * @enum msgtype
 * @brief MESSAGE types
 */
enum msgtype {
    /* 
     * When hard interrupt occurs, a msg (with type==HARD_INT) will
     * be sent to some tasks
     */
    HARD_INT = 1,

    /* SYS task */
    GET_TICKS,
};

#define RETVAL u.m3.m3i1

#endif /* _ORANGES_PROC_H_ */