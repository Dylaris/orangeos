/*
 * Common type
 */

#ifndef _ORANGES_TYPE_H_
#define _ORANGES_TYPE_H_

typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char  u8;

typedef void (*int_handler)(void);      /* Exception handler */
typedef void (*task_func)(void);        /* Function pointer that task executes */
typedef void (*irq_handler)(int irq);   /* Hardware interrupt handler */
typedef void *system_call;

#endif /* _ORANGES_TYPE_H_ */