#include "type.h"
#include "const.h"
#include "protect.h"

PUBLIC void *memcpy(void *pDst, void *pSrc, int iSize);
PUBLIC void disp_str(char *info);

PUBLIC u8 gdt_ptr[6];   /* 0-15: Limit  16-47: Base */
PUBLIC DESCRIPTOR gdt[GDT_SIZE];

PUBLIC void cstart()
{
    disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
		     "-----\"cstart\" begins-----\n");

    /* Get info from gdt_ptr */
    u16 *p_gdt_limit = (u16 *) (&gdt_ptr[0]);
    u32 *p_gdt_base = (u32 *) (&gdt_ptr[2]);

    /* Copy old GDT in loader to new GDT in kernel */
    memcpy(&gdt, (void *) *p_gdt_base, *p_gdt_limit + 1);

    /* Modify gdt_ptr */
    *p_gdt_limit = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
    *p_gdt_base = (u32) &gdt;

    disp_str("-----\"cstart\" ends-----\n");
}