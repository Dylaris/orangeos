/*
 * Constant
 */

#ifndef _ORANGES_CONST_H_
#define _ORANGES_CONST_H_

#define PUBLIC
#define PRIVATE static
#define EXTERN  extern

#define GDT_SIZE 128
#define IDT_SIZE 256

/* 8259A interrupt controller port */
#define INT_M_CTL       0x20
#define INT_M_CTLMASK   0x21
#define INT_S_CTL       0xA0
#define INT_S_CTLMASK   0xA1

/* IRQ entry point */
#define INT_VECTOR_IRQ0 0x20
#define INT_VECTOR_IRQ8 0x28

#endif /* _ORANGES_CONST_H_ */