#include "proto.h"
#include "asm/pic.h"

PUBLIC void init_clock(void)
{
    put_irq_handler(CLOCK_IRQ, clock_handler);
    enable_irq(CLOCK_IRQ);
}