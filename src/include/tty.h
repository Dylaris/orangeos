#ifndef _ORANGES_TTY_H_
#define _ORANGES_TTY_H_

#include "type.h"

#define TTY_IN_BYTES 256
#define NR_CONSOLES  3

struct s_console;

typedef struct s_tty {
    u32 in_buf[TTY_IN_BYTES];   /* Input buffer of tty */
    u32 *p_inbuf_head;          /* Point to the next idle position in input buffer */
    u32 *p_inbuf_tail;          /* Point to the key, which should be processed */
    int inbuf_count;            /* Count of bytes in input buffer */

    struct s_console *p_console;
} TTY;

#endif /* _ORANGES_TTY_H_ */