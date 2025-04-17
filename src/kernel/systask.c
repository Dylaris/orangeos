#include "sys/const.h"
#include "sys/proc.h"
#include "sys/proto.h"
#include "sys/symbol.h"

PUBLIC void task_sys(void)
{
    MESSAGE msg;
    while (1) {
        send_recv(RECEIVE, ANY, &msg);
        int src = msg.source;

        switch (msg.type) {
        case GET_TICKS:
            msg.RETVAL = ticks;
            send_recv(SEND, src, &msg);
            break;

        default:
            panic("unknnown msg type");
            break;
        }
    }
}