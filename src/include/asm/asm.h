/*
 * Stuff about x86 CPU 
 */
#ifndef _ORANGES_ASM_H_
#define _ORANGES_ASM_H_

/* Size of GDT/IDT/LDT */
#define GDT_SIZE 128
#define IDT_SIZE 256
#define LDT_SIZE 2      /* Each task has a LDT (has two descriptors) */

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

#define DA_DR       0x90 	/* Present read-only data segment type value */
#define DA_DRW      0x92 	/* Present read-write data segment attribute value */
#define DA_DRWA     0x93 	/* Present accessed read-write data segment type value */
#define DA_C        0x98 	/* Present execute-only code segment attribute value */
#define DA_CR       0x9A 	/* Present executable-readable code segment attribute value */
#define DA_CCO      0x9C 	/* Present execute-only consistent code segment attribute value */
#define DA_CCOR     0x9E 	/* Present executable-readable consistent code segment attribute value */

#define DA_LDT      0x82 	/* Local Descriptor Table segment type value */
#define DA_TaskGate 0x85 	/* Task Gate type value */
#define DA_386TSS   0x89 	/* Available 386 Task State Segment type value */
#define DA_386CGate 0x8C 	/* 386 Call Gate type value */
#define DA_386IGate 0x8E 	/* 386 Interrupt Gate type value */
#define DA_386TGate 0x8F 	/* 386 Trap Gate type value */

/* Type of selector */
#define SA_RPL_MASK (1 << 2 - 1)
#define SA_RPL0     0
#define SA_RPL1     1
#define SA_RPL2     2
#define SA_RPL3     3

#define SA_TI_MASK  (1 << 2)
#define SA_TIG      (0 << 2)
#define SA_TIL      (1 << 2)

/*=====================================================================*/

typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char  u8;

typedef struct s_descriptor {
 	u16 limit_low;
 	u16 base_low;
 	u8  base_mid;
 	u8  attr1;
 	u8  limit_high_attr2;
 	u8  base_high;
} DESCRIPTOR;

typedef struct s_gate {
 	u16 offset_low;
 	u16 selector;
 	u8  dcount; /* Count of ouble word arg needed to be copyed in call gate */
 	u8  attr;
 	u16 offset_high;
} GATE;

typedef struct s_tss {
	u32 backlink;
	u32 esp0; 	/* stack pointer to use during interrupt */
	u32 ss0;
	u32 esp1;
	u32 ss1;
	u32 esp2;
	u32 ss2;
	u32 cr3;
	u32 eip;
	u32 flags;
	u32 eax;
	u32 ecx;
	u32 edx;
	u32 ebx;
	u32 esp;
	u32 ebp;
	u32 esi;
	u32 edi;
	u32 es;
	u32 cs;
	u32 ss;
	u32 ds;
	u32 fs;
	u32 gs;
	u32 ldt;
	u16 trap;
	u16 iobase;
} TSS;

#endif /* _ORANGES_ASM_H_ */
