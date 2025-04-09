#include "type.h"
#include "const.h"
#include "proto.h"
#include "global.h"
#include "proc.h"

PUBLIC void clock_handler(int irq)
{
    ticks++;

    if (k_reenter != 0) return;

    p_proc_ready++;
    if (p_proc_ready >= proc_table + NR_TASKS)
        p_proc_ready = proc_table;
}

PUBLIC void milli_delay(int milli_sec)
{
    int t = get_ticks();
    while (((get_ticks() - t) * 1000 / HZ) < milli_sec) {}
}