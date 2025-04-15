#include "asm/pic.h"
#include "asm/pit.h"
#include "sys/proto.h"
#include "sys/symbol.h"

PUBLIC void init_clock(void)
{
    put_irq_handler(CLOCK_IRQ, clock_handler);
    enable_irq(CLOCK_IRQ);
}

PUBLIC void clock_handler(int irq)
{
    ticks++;
    p_proc_ready->ticks--;

    if (k_reenter != 0) return;
    if (p_proc_ready->ticks > 0) return;

    schedule();
}

PUBLIC void milli_delay(int milli_sec)
{
    // int t = get_ticks();
    // while (((get_ticks() - t) * 1000 / HZ) < milli_sec) {}
}
