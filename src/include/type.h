/*
 * Type (basic and protect mode)
 */

#ifndef _ORANGES_TYPE_H_
#define _ORANGES_TYPE_H_

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
	u32 esp0; /* stack pointer to use during interrupt */
	u32 ss0; /*   "   segment  "  "    "        "     */
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

typedef void (*int_handler)(void); 		/* Exception handler */
typedef void (*task_func)(void); 		/* Function pointer that task executes */
typedef void (*irq_handler)(int irq); 	/* Hardware interrupt handler */
typedef void *system_call;

#endif /* _ORANGES_TYPE_H_ */