/*
 * Function prototype
 */

#ifndef _ORANGES_PROTO_H
#define _ORANGES_PROTO_H

#include "type.h"
#include "const.h"

/* kernel/pic.c */
PUBLIC void init_8259A(void);

/* kernel/start.c */
PUBLIC void cstart(void);

/* kernel/trap.c */
PUBLIC void exception_handler(int vec_no, int err_code, int eip, int cs, int eflags);

/* lib/klib.c */
PUBLIC char *itoa(char *buf, int num);
PUBLIC void disp_int(int input);

/* lib/kliba.asm */
PUBLIC void out_byte(u16 port, u8 value);
PUBLIC u8   in_byte(u16 port);
PUBLIC void disp_str(char *str);
PUBLIC void disp_color_str(char *str, int color);

/* lib/string.asm */
PUBLIC void *memcpy(void *dst, void *src, int size);

#endif /* _ORANGES_PROTO_H */