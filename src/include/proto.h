/*
 * Function prototype
 */

#ifndef _ORANGES_PROTO_H
#define _ORANGES_PROTO_H

#include "type.h"
#include "const.h"

/* Virtual address to physics address */
#define va2pa(seg_base, va) ((u32) (seg_base) + (u32) (va))

/* kernel/pic.c */
PUBLIC void init_pic(void);
PUBLIC void spurious_irq(int irq);

/* kernel/start.c */
PUBLIC void cstart(void);

/* kernel/main.c */
PUBLIC int kernel_main(void);
PUBLIC void TestA(void);

/* kernel/trap.c */
PUBLIC void exception_handler(int vec_no, int err_code, int eip, int cs, int eflags);
PUBLIC void init_prot(void);
PUBLIC u32 seg2phys(u16 seg);

/* kernel/kernel.asm */
PUBLIC void restart(void);

/* lib/klib.c */
PUBLIC char *itoa(char *buf, int num);
PUBLIC void disp_int(int input);
PUBLIC void delay(int time);

/* lib/kliba.asm */
PUBLIC void out_byte(u16 port, u8 value);
PUBLIC u8   in_byte(u16 port);
PUBLIC void disp_str(char *str);
PUBLIC void disp_color_str(char *str, int color);

/* lib/string.asm */
PUBLIC void *memcpy(void *dst, void *src, int size);
PUBLIC void memset(void* dst, char ch, int size);

#endif /* _ORANGES_PROTO_H */