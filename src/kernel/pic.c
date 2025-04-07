#include "proto.h"

/* 8258A */
PUBLIC void init_pic(void)
{
    out_byte(INT_M_CTL, 0x11);      /* ICW 1 to master */ 
    out_byte(INT_S_CTL, 0x11);      /* ICW 1 to slave */
    out_byte(INT_M_CTLMASK, INT_VECTOR_IRQ0); /* ICW 2 to master */
    out_byte(INT_S_CTLMASK, INT_VECTOR_IRQ8); /* ICW 2 to slave */
    out_byte(INT_M_CTLMASK, 0x4);   /* ICW 3 to master */
    out_byte(INT_S_CTLMASK, 0x2);   /* ICW 3 to slave */
    out_byte(INT_M_CTLMASK, 0x1);   /* ICW 4 to master */
    out_byte(INT_S_CTLMASK, 0x1);   /* ICW 4 to slave */
    out_byte(INT_M_CTLMASK, 0xFD);  /* OCW 1 to master */
    out_byte(INT_S_CTLMASK, 0xFF);  /* OCW 1 to slave */
}

PUBLIC void spurious_irq(int irq)
{
    disp_str("spurisous_irq: ");
    disp_int(irq);
    disp_str("\n");
}