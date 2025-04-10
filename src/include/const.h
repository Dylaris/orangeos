/*
 * Constant (basic and protect mode)
 */

#ifndef _ORANGES_CONST_H_
#define _ORANGES_CONST_H_

/* Scope */
#define PUBLIC
#define PRIVATE static
#define EXTERN  extern

/* Size of GDT/IDT/LDT */
#define GDT_SIZE 128
#define IDT_SIZE 256
#define LDT_SIZE 2      /* Each task has a LDT (has two descriptors) */

/* 8259A interrupt controller port */
#define INT_M_CTL       0x20
#define INT_M_CTLMASK   0x21
#define INT_S_CTL       0xA0
#define INT_S_CTLMASK   0xA1

/* 8259A IRQ entry */
#define INT_VECTOR_IRQ0 0x20
#define INT_VECTOR_IRQ8 0x28

/* Interrupt vector (exception) */
#define INT_VECTOR_DIVIDE       0x0
#define INT_VECTOR_DEBUG        0x1
#define INT_VECTOR_NMI          0x2
#define INT_VECTOR_BREAKPOINT   0x3
#define INT_VECTOR_OVERFLOW     0x4
#define INT_VECTOR_BOUNDS       0x5
#define INT_VECTOR_INVAL_OP     0x6
#define INT_VECTOR_COPROC_NOT   0x7
#define INT_VECTOR_DOUBLE_FAULT 0x8
#define INT_VECTOR_COPROC_SEG   0x9
#define INT_VECTOR_INVAL_TSS    0xA
#define INT_VECTOR_SEG_NOT      0xB
#define INT_VECTOR_STACK_FAULT  0xC
#define INT_VECTOR_PROTECTION   0xD
#define INT_VECTOR_PAGE_FAULT   0xE
#define INT_VECTOR_COPROC_ERR   0x10

/* Interrupt vector (IRQ <=> hardware interrupt) */
#define NR_IRQ          16 
#define CLOCK_IRQ       0
#define KEYBOARD_IRQ    1
#define CASCADE_IRQ     2   /* cascade enable for 2nd AT controller */
#define ETHER_IRQ       3   /* default ethernet interrupt vector */
#define SECONDARY_IRQ   3   /* RS232 interrupt vector for port 2 */
#define RS232_IRQ       4   /* RS232 interrupt vector for port 1 */
#define XT_WINI_IRQ     5   /* xt winchester */
#define FLOPPY_IRQ      6   /* floppy disk */
#define PRINTER_IRQ     7
#define AT_WINI_IRQ     14  /* at winchester */

/* Interrupt vector (system call) */
#define INT_VECTOR_SYS_CALL     0x90

/* Privilege */
#define PRIVILEGE_KRNL  0
#define PRIVILEGE_TASK  1
#define PRIVILEGE_USER  3

/* RPL */
#define RPL_KRNL    SA_RPL0
#define RPL_TASK    SA_RPL1
#define RPL_USER    SA_RPL3

/* GDT descriptor index */
#define INDEX_DUMMY     0 // ┓
#define INDEX_FLAT_C    1 // ┣ Defined in loader
#define INDEX_FLAT_RW   2 // ┃
#define INDEX_VIDEO     3 // ┛
#define INDEX_TSS       4
#define INDEX_LDT_FIRST 5

/* GDT selector */
#define SELECTOR_DUMMY      (INDEX_DUMMY<<3)           // ┓
#define SELECTOR_FLAT_C     (INDEX_FLAT_C<<3)          // ┣ Defined in loader
#define SELECTOR_FLAT_RW    (INDEX_FLAT_RW<<3)         // ┃
#define SELECTOR_VIDEO      ((INDEX_VIDEO<<3)|SA_RPL3) // ┛<-- RPL=3
#define SELECTOR_TSS        (INDEX_TSS<<3)
#define SELECTOR_LDT_FIRST  (INDEX_LDT_FIRST<<3)

#define SELECTOR_KERNEL_CS  SELECTOR_FLAT_C
#define SELECTOR_KERNEL_DS  SELECTOR_FLAT_RW
#define SELECTOR_KERNEL_GS  SELECTOR_VIDEO

/* LDT descriptor index */
#define INDEX_LDT_C     0
#define INDEX_LDT_RW    1

/* Type of descriptor */
#define DA_32       0x4000  /* 32 位段    */
#define DA_LIMIT_4K 0x8000  /* 段界限粒度为 4K 字节   */
#define DA_DPL0     0x00    /* DPL = 0    */
#define DA_DPL1     0x20    /* DPL = 1    */
#define DA_DPL2     0x40    /* DPL = 2    */
#define DA_DPL3     0x60    /* DPL = 3    */

#define DA_DR       0x90 /* 存在的只读数据段类型值  */
#define DA_DRW      0x92 /* 存在的可读写数据段属性值  */
#define DA_DRWA     0x93 /* 存在的已访问可读写数据段类型值 */
#define DA_C        0x98 /* 存在的只执行代码段属性值  */
#define DA_CR       0x9A /* 存在的可执行可读代码段属性值  */
#define DA_CCO      0x9C /* 存在的只执行一致代码段属性值  */
#define DA_CCOR     0x9E /* 存在的可执行可读一致代码段属性值 */

#define DA_LDT      0x82 /* 局部描述符表段类型值   */
#define DA_TaskGate 0x85 /* 任务门类型值    */
#define DA_386TSS   0x89 /* 可用 386 任务状态段类型值  */
#define DA_386CGate 0x8C /* 386 调用门类型值   */
#define DA_386IGate 0x8E /* 386 中断门类型值   */
#define DA_386TGate 0x8F /* 386 陷阱门类型值   */

/* Type of selector */
#define SA_RPL_MASK (1 << 2 - 1)
#define SA_RPL0     0
#define SA_RPL1     1
#define SA_RPL2     2
#define SA_RPL3     3

#define SA_TI_MASK  (1 << 2)
#define SA_TIG      (0 << 2)
#define SA_TIL      (1 << 2)

/* System call */
#define NR_SYS_CALL 1

/* 8253/8254 pit (programmable interval table) */
#define TIMER0          0x40        /* I/O port for timer channel 0 */
#define TIMER_MODE      0x43        /* I/O port for timer mode control */
#define RATE_GENERATOR  0x34        /* 00-11-010-0 
                                     * counter0 - LSB then MSB - rate generator - binary */
#define TIMER_FREQ      1193182L    /* Clock frequency for timer in PC and AT */
#define HZ              100         /* Clock frequency (software settable on IBM-PC */

#define TRUE    1
#define FALSE   0

/*
 * Color
 *
 * e.g. MAKE_COLOR(BLUE, RED)
 *      MAKE_COLOR(BLACK, RED) | BRIGHT
 *      MAKE_COLOR(BLACK, RED) | BRIGHT | FLASH
 */
#define BLACK   0x0     /* 0000 */
#define WHITE   0x7     /* 0111 */
#define RED     0x4     /* 0100 */
#define GREEN   0x2     /* 0010 */
#define BLUE    0x1     /* 0001 */
#define FLASH   0x80    /* 1000 0000 */
#define BRIGHT  0x08    /* 0000 1000 */

#define MAKE_COLOR(b, f) ((b<<4) | f) /* MAKE_COLOR(Background, Foreground) */

#endif /* _ORANGES_CONST_H_ */