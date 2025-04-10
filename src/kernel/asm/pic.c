#include "asm/pic.h"
#include "inc.h"

PUBLIC void init_pic(void)
{
    /* Initialize 8258A */
    out_byte(INT_M_CTL, 0x11);      /* ICW 1 to master */ 
    out_byte(INT_S_CTL, 0x11);      /* ICW 1 to slave */
    out_byte(INT_M_CTLMASK, INT_VECTOR_IRQ0); /* ICW 2 to master */
    out_byte(INT_S_CTLMASK, INT_VECTOR_IRQ8); /* ICW 2 to slave */
    out_byte(INT_M_CTLMASK, 0x4);   /* ICW 3 to master */
    out_byte(INT_S_CTLMASK, 0x2);   /* ICW 3 to slave */
    out_byte(INT_M_CTLMASK, 0x1);   /* ICW 4 to master */
    out_byte(INT_S_CTLMASK, 0x1);   /* ICW 4 to slave */
    out_byte(INT_M_CTLMASK, 0xFF);  /* OCW 1 to master */
    out_byte(INT_S_CTLMASK, 0xFF);  /* OCW 1 to slave */

    /* Initialize irq table */
    for (int i = 0; i < NR_IRQ; i++)
        irq_table[i] = spurious_irq;
}

PUBLIC void put_irq_handler(int irq, irq_handler handler)
{
    disable_irq(irq);   /* Disable interrupt before setting new handler */
    irq_table[irq] = handler;
}