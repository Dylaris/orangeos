#include "type.h"
#include "const.h"
#include "protect.h"

PUBLIC void *memcpy(void *dst, void *src, int size);
PUBLIC void disp_str(char * str);

PUBLIC u8 gdt_ptr[6];
PUBLIC DESCRIPTOR gdt[GDT_SIZE];

PUBLIC void cstart()
{
    disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
         "-----\"cstart\" begins-----\n");

    u16 *p_gdt_limit = (u16 *) (&gdt_ptr[0]);
    u32 *p_gdt_base  = (u32 *) (&gdt_ptr[2]);
    memcpy(&gdt,*p_gdt_base, *p_gdt_limit + 1);
    *p_gdt_limit = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
    *p_gdt_base = (u32) &gdt;

    disp_str("-----\"cstart\" ends-----\n");
}
