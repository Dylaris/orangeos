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

/**
 * @brief Print something on the control console of given process
 * @param str String to be print
 * @param p_proc The caller
 * @return Zero is success or hlt the system
 */
PUBLIC int sys_printx(int _unused1, int _unused2, char *str, PROCESS *p_proc)
{
    char reenter_err[] = "? k_reenter is incorrect for unknown reason";
    reenter_err[0] = MAG_CH_PANIC;

    /**
     * @note Code in both Ring 0 and Ring 1~3 may invoke printx().
     * If this happens in Ring 0, no linear-physical address mapping
     * is needed.
     * 
     * @attention The value of 'k_reenter' is tricky here. When
     *  -# printx() is called in Ring 0
     *     - k_reenter > 0. When code in Ring 0 calls printx(),
     *       an 'interrupt re-enter' will occur (printx() generates
     *       a software interrupt). Thus 'k_reenter' will be increased
     *       by 'kernel.asm::save' and be greater than 0.
     *  -# printx() is called in Ring 1~3
     *     - k_reenter == 0.
     */

    const char *p;
    /* printx() called in Ring 1~3 */
    if (k_reenter == 0) p = va2la(proc2pid(p_proc), str);
    /* printx() called in Ring 0 */
    else if (k_reenter > 0) p = str;
    /* This should not happen */
    else p = reenter_err;

    /**
     * @note if assertion fails in any TASK, the system will be halted;
     * if it fails in a USER PROC, it'll return like any normal syscall does.
     */
    if ((*p == MAG_CH_PANIC) ||                   /* Not NR_TASKS+NR_PROCS */
        (*p == MAG_CH_ASSERT && p_proc_ready < &proc_table[NR_TASKS])) {
        disable_int();
        char *v = (char *) V_MEM_BASE;
        const char *q = p + 1;  /* +1: Skip the magic char */
        while (v < (char *) (V_MEM_BASE + V_MEM_SIZE)) {
            *v++ = *q++;
            *v++ = RED_CHAR;
            if (!*q) {
                while (((int) v - V_MEM_BASE) % (SCREEN_WIDTH * 16)) {
                    v++;
                    *v++ = GRAY_CHAR;
                }
            }
        }

        __asm__ __volatile__ ("hlt");
    }

    char ch;
    while ((ch = *p++) != 0) {
        if (ch == MAG_CH_PANIC || ch == MAG_CH_ASSERT)
            continue;   /* Skip the magic char */
        out_char(tty_table[p_proc->nr_tty].p_console, ch);
    }

    return 0;
}