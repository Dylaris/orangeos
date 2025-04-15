#include "asm/vga.h"
#include "sys/const.h"
#include "sys/tty.h"
#include "sys/proto.h"
#include "sys/symbol.h"

#define TTY_FIRST   (tty_table)
#define TTY_END     (tty_table + NR_CONSOLES) 

PRIVATE void init_tty(TTY *p_tty);
PRIVATE void tty_do_read(TTY *p_tty);
PRIVATE void tty_do_write(TTY *p_tty);
PRIVATE void put_key(TTY *p_tty, u32 key);

PUBLIC void task_tty(void)
{
    TTY *p_tty;

    init_keyboard();

    for (p_tty = TTY_FIRST; p_tty < TTY_END; p_tty++)
        init_tty(p_tty);
    select_console(0);
    
    while (1) {
        for (p_tty = TTY_FIRST; p_tty < TTY_END; p_tty++) {
            tty_do_read(p_tty);
            tty_do_write(p_tty);
        }
    }
}

PUBLIC void in_process(TTY *p_tty, u32 key)
{
    if (!(key & FLAG_EXT)) {    /* Printable */
        put_key(p_tty, key);
    } else {
        int raw_code = key & MASK_RAW;
        switch (raw_code) {
        case ENTER:
            put_key(p_tty, '\n');
            break;

        case BACKSPACE:
            put_key(p_tty, '\b');
            break;

        case UP:
            /* Shift + UP */
            if (key & FLAG_SHIFT_L || key & FLAG_SHIFT_R)
                scroll_screen(p_tty->p_console, SCR_UP);
            break;

        case DOWN:
            /* Shift + DOWN */
            if (key & FLAG_SHIFT_L || key & FLAG_SHIFT_R)
                scroll_screen(p_tty->p_console, SCR_DN);
            break;

        case F1:
        case F2:
        case F3:
        case F4:
        case F5:
        case F6:
        case F7:
        case F8:
        case F9:
        case F10:
        case F11:
        case F12:
            /* Alt + Fn */
            if (key & FLAG_ALT_L || key & FLAG_ALT_R)
                select_console(raw_code - F1);
            break;

        default:
            break;
        }
    }
}

PRIVATE void init_tty(TTY *p_tty)
{
    p_tty->inbuf_count = 0;
    p_tty->p_inbuf_head = p_tty->p_inbuf_tail = p_tty->in_buf;

    init_screen(p_tty);
}

PRIVATE void tty_do_read(TTY *p_tty)
{
    if (is_current_console(p_tty->p_console))
        keyboard_read(p_tty);
}

PRIVATE void tty_do_write(TTY *p_tty)
{
    if (p_tty->inbuf_count) {
        char ch = *p_tty->p_inbuf_tail;
        p_tty->p_inbuf_tail++;
        if (p_tty->p_inbuf_tail == p_tty->in_buf + TTY_IN_BYTES)
            p_tty->p_inbuf_tail = p_tty->in_buf;
        p_tty->inbuf_count--;

        out_char(p_tty->p_console, ch);
    }
}

PRIVATE void put_key(TTY *p_tty, u32 key)
{
    if (p_tty->inbuf_count < TTY_IN_BYTES) {
        *p_tty->p_inbuf_head = key;
        p_tty->p_inbuf_head++;
        if (p_tty->p_inbuf_head == p_tty->in_buf + TTY_IN_BYTES)
            p_tty->p_inbuf_head = p_tty->in_buf;
        p_tty->inbuf_count++;
    }
}

PUBLIC void tty_write(TTY *p_tty, char *buf, int len)
{
    char *p = buf;
    for (int i = 0; i < len; i++)
        out_char(p_tty->p_console, *p++);
}

PUBLIC int sys_write(char *buf, int len, PROCESS *p_proc)
{
    /* Need p_proc to get its control tty */
    tty_write(&tty_table[p_proc->nr_tty], buf, len);
    return 0;
}