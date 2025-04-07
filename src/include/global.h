/*
 * Global variable
 */

#ifndef _ORANGES_GLOBAL_H
#define _ORANGES_GLOBAL_H

#include "type.h"
#include "const.h"

#ifdef GLOBAL_VARIABLE_HERE
#undef  EXTERN
#define EXTERN
#endif /* GLOBAL_VARIABLE_HERE */

EXTERN int          disp_pos;
EXTERN u8           gdt_ptr[6];
EXTERN DESCRIPTOR   gdt[GDT_SIZE];
EXTERN u8           idt_ptr[6];
EXTERN GATE         idt[IDT_SIZE];

#endif /* _ORANGES_GLOBAL_H */