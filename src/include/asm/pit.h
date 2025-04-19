/*
 * Stuff about PIT
 */

#ifndef _ORANGES_ASM_PIT_H_
#define _ORANGES_ASM_PIT_H_

/* 8253/8254 pit (programmable interval table) */
#define TIMER0          0x40        /* I/O port for timer channel 0 */
#define TIMER_MODE      0x43        /* I/O port for timer mode control */
#define RATE_GENERATOR  0x34        /* 00-11-010-0 
                                     * counter0 - LSB then MSB - rate generator - binary */
#define TIMER_FREQ      1193182L    /* Clock frequency for timer in PC and AT */
#define HZ              100         /* Clock frequency (software settable on IBM-PC */

#endif /* _ORANGES_ASM_PIT_H_ */