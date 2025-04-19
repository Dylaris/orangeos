#include "sys/const.h"
#include "sys/symbol.h"
#include "sys/proc.h"
#include "sys/proto.h"
#include "string.h"

PRIVATE void block(PROCESS *p_proc);
PRIVATE void unblock(PROCESS *p_proc);
PRIVATE int deadlock(int src, int dest);
PRIVATE int msg_send(PROCESS *current, int dest, MESSAGE *p_msg);
PRIVATE int msg_receive(PROCESS *current, int src, MESSAGE *p_msg);

PUBLIC void schedule(void)
{
    PROCESS *p;
    int greatest_ticks = 0;

    while (!greatest_ticks) {
        /* Schedule the process with most ticks to run */
        for (p = &FIRST_PROC; p <= &LAST_PROC; p++) {
            if (p->p_flags == RUNNING) {
                if (p->ticks > greatest_ticks) {
                    greatest_ticks = p->ticks;
                    p_proc_ready = p;
                }
            }
        }
        /* (greatest_ticks == 0) means every process has no time to run */
        if (!greatest_ticks) {
            for (p = &FIRST_PROC; p <= &LAST_PROC; p++) {
                if (p->p_flags == RUNNING)
                    p->ticks = p->priority;
            }
        }
    }
}

/**
 * @brief <Ring 1~3> IPC syscall
 * It is an encapsulation of 'sendrec', * invoking 'sendrec' directly should be avoided
 * @param function SEND or RECEIVE
 * @param src_dest To/From whom the message is transferred
 * @param p_msg    Ptr to the MESSAGE body
 * @return Always 0
 */
PUBLIC int send_recv(int funciton, int src_dest, MESSAGE *p_msg)
{
    int ret = 0;

    if (funciton == RECEIVE)
        memset(p_msg, 0, sizeof(MESSAGE));

    switch (funciton) {
    case BOTH:
        ret = sendrec(SEND, src_dest, p_msg);
        if (ret == 0)
            ret = sendrec(RECEIVE, src_dest, p_msg);
        break;

    case SEND:
    case RECEIVE:
        ret = sendrec(funciton, src_dest, p_msg);
        break;

    default:
        assert((funciton == BOTH) ||
               (funciton == SEND) || (funciton == RECEIVE));
        break;
    }

    return ret;
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

/**
 * @brief <Ring 0~3> Clear up a MESSAGE by setting each byte to 0
 * @param p_msg The message to be cleared
 * @return 
 */
PUBLIC void reset_msg(MESSAGE *p_msg)
{
    memset(p_msg, 0, sizeof(MESSAGE));
}

/**
 * @brief <Ring 0> This routine is called after 'p_flags' has been set (!= 0), it
 * calls 'schedule()' to choose another process as the 'proc_ready'
 * @attention This routine does not change 'p_flags'. Make sure the 'p_flags'
 * of the process to be blocked has been set properly
 * @param p_proc The process to be blocked
 * @return 
 */
PRIVATE void block(PROCESS *p_proc)
{
    assert(p_proc->p_flags != RUNNING);   /* Not running */
    schedule();
}

/**
 * @brief <Ring 0> This is a dummy routine. It does nothing actually. When it is
 * called, the 'p_flags' should have been cleared (== 0)
 * @param p_proc The process to be unblocked
 * @return 
 */
PRIVATE void unblock(PROCESS *p_proc)
{
    assert(p_proc->p_flags == RUNNING);
}

/**
 * @brief <Ring 0> Check whether it is safe to send a message from src to dest.
 * The routine will detect if the messaging graph contains a cycle. For instance
 * , if we have processes trying to send message like this: A -> B -> C -> A,
 * then a deadlock occrus, because all of them will wait forever. If no cycles
 * detected, it is considered as safe.
 * @param src  Who wants to send message
 * @param dest To whom the message is sent
 * @return Zero is success
 */
PRIVATE int deadlock(int src, int dest)
{
    PROCESS *p_proc = proc_table + dest;
    while (1) {
        /* Check if it is a cycle */
        if (p_proc->p_flags & SENDING) {
            if (p_proc->p_sendto == src) {
                /* Print the chain */
                p_proc = proc_table + dest;
                printl("=_=%s", p_proc->p_name);
                do {
                    assert(p_proc->p_msg);
                    p_proc = proc_table + p_proc->p_sendto;
                    printl("->%s", p_proc->p_name);
                } while (p_proc != proc_table + src);
                printl("=_=");

                return 1;
            }
            p_proc = proc_table + p_proc->p_sendto;
        }
        /* Not a cycle */
        else {
            break;
        }
    }
    return 0;
}

/**
 * @brief <Ring 0> Send a message to the dest process. If dest is blocked waiting for 
 * the message, copy the message to it and unblock dest. Ohterwise the caller will be
 * blocked and appended to the dest's sending queue.
 * @param current The caller, the sender
 * @param dest    The whom the message is sent
 * @param p_msg   The message to be sent
 * @return Zero is success
 */
PRIVATE int msg_send(PROCESS *current, int dest, MESSAGE *p_msg)
{
    PROCESS *p_sender = current;
    PROCESS *p_recver = proc_table + dest;

    assert(proc2pid(p_sender) != dest);

    /* Check for deadlock here */
    if (deadlock(proc2pid(p_sender), dest)) {
        panic(">>DEADLOCK<< %s->%s", p_sender->p_name, p_recver->p_name);
    }

    /* Receiver is waiting for the message */
    if ((p_recver->p_flags & RECEIVING) &&
        (p_recver->p_recvfrom == proc2pid(p_sender) ||
         p_recver->p_recvfrom == ANY)) {
        assert(p_recver->p_msg);
        assert(p_msg);

        /* Copy the message in sender to receiver */
        phys_copy(va2la(dest, p_recver->p_msg), 
            va2la(proc2pid(p_sender), p_msg), sizeof(MESSAGE));
        p_recver->p_msg = NULL; /* Avoid to use old p_msg (it just set p_msg to 0, not real msg body) */
        p_recver->p_flags &= ~RECEIVING; /* Receiver has received the msg */
        p_recver->p_recvfrom = NO_TASK;

        unblock(p_recver);

        assert(p_recver->p_flags == RUNNING);
        assert(p_recver->p_msg == NULL);
        assert(p_recver->p_recvfrom == NO_TASK);
        assert(p_recver->p_sendto == NO_TASK);

        assert(p_sender->p_flags == RUNNING);
        assert(p_sender->p_msg == NULL);
        assert(p_sender->p_recvfrom == NO_TASK);
        assert(p_sender->p_sendto == NO_TASK);
    }
    /* Receiver is not waiting for the message */
    else {
        p_sender->p_flags |= SENDING;
        assert(p_sender->p_flags == SENDING);
        p_sender->p_sendto = dest;
        p_sender->p_msg = p_msg;

        /* Append to the sending queue */
        PROCESS *p;
        if (p_recver->q_sending) {
            p = p_recver->q_sending;
            while (p->next_sending)
                p = p->next_sending;
            p->next_sending = p_sender;
        }
        else {
            p_recver->q_sending = p_sender;
        }
        p_sender->next_sending = NULL;

        block(p_sender);

        assert(p_sender->p_flags == SENDING);
        assert(p_sender->p_msg != NULL);
        assert(p_sender->p_recvfrom == NO_TASK);
        assert(p_sender->p_sendto == dest);
    }

    return 0;
}

/**
 * @brief <Ring 0> Try to get a message from the src process. If sec is blocked sending
 * the message, copy the message from it and unblock src. Otherwise the caller
 * will be blocked
 * @param current The caller, the receiver
 * @param src     From whom the message will be sent
 * @param p_msg   The message body to accept the message
 * @return 
 */
PRIVATE int msg_receive(PROCESS *current, int src, MESSAGE *p_msg)
{
    PROCESS *p_recver = current;
    PROCESS *p_sender = NULL;
    PROCESS *p_prev = NULL;
    int copyok = 0;

    assert(proc2pid(p_recver) != src);

    /* There is an interrupt needs p_recver's handling and p_recver is ready to handle it */
    if ((p_recver->has_int_msg) &&
       ((src == ANY) || (src == INTERRUPT))) {

        MESSAGE msg;
        reset_msg(&msg);
        msg.source = INTERRUPT;
        msg.type = HARD_INT;
        assert(p_msg);
        phys_copy(va2la(proc2pid(p_recver), p_msg), &msg, sizeof(MESSAGE));

        p_recver->has_int_msg = 0;

        assert(p_recver->p_flags == RUNNING);
        assert(p_recver->p_msg == NULL);
        assert(p_recver->p_sendto == NO_TASK);
        assert(p_recver->has_int_msg == 0);

        return 0;
    }

    /* Arrives here if no interrupt for p_recver */

    /* p_recver is ready to receive messages from ANY process, 
       we'll check the sending queue and pick the first process in it. */
    if (src == ANY) {
        if (p_recver->q_sending) {
            p_sender = p_recver->q_sending;
            copyok = 1;

            assert(p_recver->p_flags == RUNNING);
            assert(p_recver->p_msg == NULL);
            assert(p_recver->p_recvfrom == NO_TASK);
            assert(p_recver->p_sendto == NO_TASK);
            assert(p_recver->q_sending != NULL);

            assert(p_sender->p_flags == SENDING);
            assert(p_sender->p_msg != NULL);
            assert(p_sender->p_recvfrom == NO_TASK);
            assert(p_sender->p_sendto == proc2pid(p_recver));
        }
    }
    /* p_recver wants to receive a message from a certain process: src */
    else {
        p_sender = &proc_table[src];

        if ((p_sender->p_flags & SENDING) &&
            (p_sender->p_sendto == proc2pid(p_recver))) {
            /* Perfect, src is sending a message to p_recver */
            copyok = 1;

            PROCESS *p = p_recver->q_sending;
            assert(p); /* p_sender must have been appended to the queue, so the queue must not be NULL */

            /* Pick the certain process */
            while (p) {
                assert(p_sender->p_flags & SENDING);
                if (proc2pid(p) == src) {
                    p_sender = p;
                    break;
                }
                p_prev = p;
                p = p->next_sending;
            }

            assert(p_recver->p_flags == RUNNING);
            assert(p_recver->p_msg == NULL);
            assert(p_recver->p_recvfrom == NO_TASK);
            assert(p_recver->p_sendto == NO_TASK);
            assert(p_recver->q_sending != NULL);

            assert(p_sender->p_flags == SENDING);
            assert(p_sender->p_msg != NULL);
            assert(p_sender->p_recvfrom == NO_TASK);
            assert(p_sender->p_sendto == proc2pid(p_recver));
        }
    }

    /* It's determied from which process the message will be copied. Note that
       this process must have been waiting for this moment in the queue, so we 
       should remove it from queue. */
    if (copyok) {
        /* Ths 1st one */
        if (p_sender == p_recver->q_sending) {
            assert(p_prev == NULL);
            p_recver->q_sending = p_sender->next_sending;
            p_sender->next_sending = NULL;
        }
        /* Not 1st one (update link info in sending queue) */
        else {
            assert(p_prev);
            p_prev->next_sending = p_sender->next_sending;
            p_sender->next_sending = NULL;
        }

        assert(p_msg);
        assert(p_sender->p_msg);

        /* Copy the message */
        phys_copy(va2la(proc2pid(p_recver), p_msg),
            va2la(proc2pid(p_sender), p_sender->p_msg), sizeof(MESSAGE));

        p_sender->p_msg = NULL;
        p_sender->p_sendto = NO_TASK;
        p_sender->p_flags &= ~SENDING;

        unblock(p_sender);
    }
    /* Nobody's sending any msg. Set p_flags so that p_recver 
       will not be scheduled until it is unblocked */
    else {
        p_recver->p_flags |= RECEIVING;
        p_recver->p_msg = p_msg;

        if (src == ANY) p_recver->p_recvfrom = ANY;
        else p_recver->p_recvfrom = proc2pid(p_sender);

        block(p_recver);

        assert(p_recver->p_flags == RECEIVING);
        assert(p_recver->p_msg != NULL);
        assert(p_recver->p_recvfrom != NO_TASK);
        assert(p_recver->p_sendto == NO_TASK);
        assert(p_recver->has_int_msg == 0);
    }

    return 0;
}

/**
 * @brief <Ring 0> Inform a process that an interrupt has occured
 * @param task_nr The task which will be informed
 */
PUBLIC void inform_int(int task_nr)
{
    PROCESS *p_proc = proc_table + task_nr;

    if ((p_proc->p_flags & RECEIVING) &&
       ((p_proc->p_recvfrom == INTERRUPT) || (p_proc->p_recvfrom == ANY))) {
        p_proc->p_msg->source = INTERRUPT;
        p_proc->p_msg->type = HARD_INT;
        p_proc->p_msg = NULL;
        p_proc->has_int_msg = 0;
        p_proc->p_flags &= ~RECEIVING;
        p_proc->p_recvfrom = NO_TASK;

        assert(p_proc->p_flags == RUNNING);
        unblock(p_proc);

        assert(p_proc->p_flags == RUNNING);
        assert(p_proc->p_msg == NULL);
        assert(p_proc->p_recvfrom == NO_TASK);
        assert(p_proc->p_sendto == NO_TASK);
    }
    else {
        p_proc->has_int_msg = 1;
    }
}