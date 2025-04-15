#include "sys/const.h"
#include "sys/symbol.h"
#include "sys/proc.h"
#include "sys/proto.h"

PRIVATE void block(PROCESS *p_proc);
PRIVATE void unblock(PROCESS *p_proc);
PRIVATE int msg_send(PROCESS *current, int dest, MESSAGE *p_msg);
PRIVATE int msg_receive(PROCESS *current, int src, MESSAGE *p_msg);
PRIVATE int deadlock(int src, int dest);

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

/**
 * @brief <Ring 0> The core routine of system call 'sendrec()'
 * @param function SEND or RECEIVE
 * @param src_dest To/From whom the message is transferred
 * @param p_msg    Ptr to the MESSAGE body
 * @param p_proc   The caller process
 * @return Zero if success
 */
PUBLIC int sys_sendrec(int function, int src_dest, MESSAGE *p_msg, PROCESS *p_proc)
{
    assert(k_reenter == 0);     /* Make sure we are not in ring0 */
    assert((src_dest >= 0 && src_dest < NR_TASKS + NR_PROCS) || 
            src_dest == ANY || src_dest == INTERRUPT);

    int ret = 0;
    int caller = proc2pid(p_proc);
    MESSAGE *mla = (MESSAGE *) va2la(caller, p_msg);
    mla->source = caller;

    assert(mla->source != src_dest);

    /* 
     * Actually we have the third message type: BOTH. However, it is not 
     * allowed to be passed to the kernel directly. Kernel doesn't know 
     * it at all. It is transformed into a SEND follwed by a RECEIVE by 'send_recv()'
     */
    if (function == SEND) {
        ret = msg_send(p_proc, src_dest, p_msg);
        if (ret != 0) return ret;
    }
    else if (function == RECEIVE) {
        ret = msg_receive(p_proc, src_dest, p_msg);
        if (ret != 0) return ret;
    }
    else {
        panic("{sys_sendrec} invalid function: "
              "%d (SEND:%d, RECEIVE:%d).", function, SEND, RECEIVE);
    }

    return 0;
}

/**
 * @brief <Ring 0~1> Virtual address -> Linear address
 * @param pid PID of the process whose address is to be calculated
 * @param va  Virtual address
 * @return The linear address for the given virtual address
 */
PUBLIC void *va2la(int pid, void *va)
{
    PROCESS *p_proc = &proc_table[pid];
    u32 seg_base = ldt_seg_linear(p_proc, INDEX_LDT_RW);
    u32 la = seg_base + (u32) va;

    if (pid < NR_TASKS + NR_PROCS) assert(la == (u32) va);
    return (void *) la;
}

/**
 * @brief <Ring 0~1> Calculate the linear address of a certain segment of a given process
 * @param p_proc Whose (the process ptr)
 * @param idx    Which (one process has more than one segmetns)
 * @return The required linear address
 */
PUBLIC int ldt_seg_linear(PROCESS *p_proc, int idx)
{
    DESCRIPTOR *p_desc = &p_proc->ldts[idx];
    return (p_desc->base_high << 24) | (p_desc->base_mid << 16) | (p_desc->base_low);
}

PRIVATE void block(PROCESS *p_proc)
{

}

PRIVATE void unblock(PROCESS *p_proc)
{

}

PRIVATE int msg_send(PROCESS *current, int dest, MESSAGE *p_msg)
{

}

PRIVATE int msg_receive(PROCESS *current, int src, MESSAGE *p_msg)
{

}

PRIVATE int deadlock(int src, int dest)
{

}
