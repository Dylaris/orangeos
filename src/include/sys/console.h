#ifndef _ORANGES_CONSOLE_H_
#define _ORANGES_CONSOLE_H_

#include "sys/const.h"

typedef struct s_console {
    unsigned int current_start_addr;    /* Current video memory address */
    unsigned int original_addr;         /* The start of video memory with current console */
    unsigned int v_mem_limit;           /* The limit of video memroy with current console */
    unsigned int cursor;                /* Current cursor position in screen */
} CONSOLE;

#define SCR_UP  1   /* Scroll forward */
#define SCR_DN -1   /* Scroll backward */ 

#define SCREEN_SIZE  (80 * 25)
#define SCREEN_WIDTH 80

#define DEFAULT_CHAR_COLOR  (MAKE_COLOR(BLACK, WHITE))
#define GRAY_CHAR           (MAKE_COLOR(BLACK, BLACK) | BRIGHT)
#define RED_CHAR            (MAKE_COLOR(BLUE, RED) | BRIGHT)

#endif /* _ORANGES_CONSOLE_H_ */