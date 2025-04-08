#include "proto.h"

/* Mask means disable the interrupt */
#define HWINT_MASK_ALL      (~((u8) 0))
#define HWINT_MASK_CLOCK    ((u8) (1 << 0))
#define HWINT_MASK_KEYBOARD ((u8) (1 << 1))

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
    out_byte(INT_M_CTLMASK, ~HWINT_MASK_KEYBOARD    /* OCW 1 to master */
                          & ~HWINT_MASK_CLOCK);
    out_byte(INT_S_CTLMASK, HWINT_MASK_ALL);        /* OCW 1 to slave */
}

PUBLIC void spurious_irq(int irq)
{
    disp_str("spurisous_irq: ");
    disp_int(irq);
    disp_str("\n");
}