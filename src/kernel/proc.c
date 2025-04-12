#include "const.h"
#include "symbol.h"
#include "proc.h"

PUBLIC void schedule(void)
{
    PROCESS *p;
    int greatest_ticks = 0;

    while (!greatest_ticks) {
        /* Schedule the process with most ticks to run */
        for (p = proc_table; p < proc_table + NR_TASKS + NR_PROCS; p++) {
            if (p->ticks > greatest_ticks) {
                greatest_ticks = p->ticks;
                p_proc_ready = p;
            }
        }
        /* (greatest_ticks == 0) means every process has no time to run */
        if (!greatest_ticks) {
            for (p = proc_table; p < proc_table + NR_TASKS + NR_PROCS; p++)
                p->ticks = p->priority;
        }
    }
}