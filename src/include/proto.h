/*
 * Function prototype
 */

#ifndef _ORANGES_PROTO_H
#define _ORANGES_PROTO_H

#include "type.h"
#include "const.h"

PUBLIC void out_byte(u16 port, u8 value);
PUBLIC u8 in_byte(u16 port);
PUBLIC void disp_str(char *str);
PUBLIC void disp_color_str(char *str, int color);
PUBLIC void *memcpy(void *dst, void *src, int size);

#endif /* _ORANGES_PROTO_H */