#ifndef _ORANGES_CONSOLE_H_
#define _ORANGES_CONSOLE_H_

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

#define DEFAULT_CHAR_COLOR 0x07

#endif /* _ORANGES_CONSOLE_H_ */