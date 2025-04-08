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

/* IRQ entry point */
#define INT_VECTOR_IRQ0 0x20
#define INT_VECTOR_IRQ8 0x28

/* Privilege */
#define PRIVILEGE_KRNL  0
#define PRIVILEGE_TASK  1
#define PRIVILEGE_USER  3

/* RPL */
#define	RPL_KRNL    SA_RPL0
#define	RPL_TASK    SA_RPL1
#define	RPL_USER    SA_RPL3

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

/* Interrupt vector */
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

#endif /* _ORANGES_CONST_H_ */