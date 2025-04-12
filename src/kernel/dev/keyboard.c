#include "const.h"
#include "type.h"
#include "symbol.h"
#include "asm/pic.h"
#include "proto.h"

PRIVATE int code_with_E0;
PRIVATE int shift_l;        /* Left shift state */ 
PRIVATE int shift_r;        /* Right shift state */ 
PRIVATE int alt_l;          /* Left alt state */ 
PRIVATE int alt_r;          /* Right alt state */ 
PRIVATE int ctrl_l;         /* Left ctrl state */ 
PRIVATE int ctrl_r;         /* Right ctrl state */ 
PRIVATE int caps_lock;      /* Caps Lock */
PRIVATE int num_lock;       /* Num Lock */
PRIVATE int scroll_lock;    /* Scroll Lock */
PRIVATE int column;         /* Column in keymap */

PRIVATE u8 get_byte_from_kbuf(void);

PUBLIC void init_keyboard(void)
{
    kb_in.count = 0;
    kb_in.p_head = kb_in.p_tail = kb_in.buf;

    code_with_E0 = caps_lock = num_lock = scroll_lock  = FALSE;
    shift_l = shift_r = FALSE;
    alt_l   = alt_r   = FALSE;
    ctrl_l  = ctrl_r  = FALSE;
    column = 0;

    put_irq_handler(KEYBOARD_IRQ, keyboard_handler);
    enable_irq(KEYBOARD_IRQ);
}

PUBLIC void keyboard_read(TTY *p_tty)
{
    u8   scan_code;
    int  make;      /* TRUE: make code    FALSE: break code */
    u32  key = 0;   /* Key macro defined in keyboard.h */
    u32  *keyrow;   /* Row at keymap */

    if (kb_in.count > 0) {
        code_with_E0 = FALSE;
        scan_code = get_byte_from_kbuf();

        if (scan_code == 0xE1) {
            u8 pausebrk_scode[] = {
                0xE1, 0x1D, 0x45,   /* Make code */
                0xE1, 0x9D, 0xC5    /* Break code */
            };
            int is_pausebreak = TRUE;

            for (int i = 1; i < 6; i++) {
                if (get_byte_from_kbuf() != pausebrk_scode[i]) {
                    is_pausebreak = FALSE;
                    break;
                }
            }
            if (is_pausebreak) key = PAUSEBREAK;
        } else if (scan_code == 0xE0) {
            scan_code = get_byte_from_kbuf();
            if (scan_code == 0x2A && get_byte_from_kbuf() == 0xE0 && get_byte_from_kbuf() == 0x37) {
                key = PRINTSCREEN;
                make = TRUE;
            }
            if (scan_code == 0xB7 && get_byte_from_kbuf() == 0xE0 && get_byte_from_kbuf() == 0xAA) {
                key = PRINTSCREEN;
                make = FALSE;
            }

            if (key == 0) code_with_E0 = TRUE;
        } 
        if (key != PAUSEBREAK && key != PRINTSCREEN) {
            make = (scan_code & FLAG_BREAK) ? FALSE : TRUE;
            keyrow = (u32 *) &keymap[(scan_code & 0x7f) * MAP_COLS];
            column = 0;

            if (shift_l || shift_r) {
                column = 1;
            }
            if (code_with_E0) {
                column = 2;
                code_with_E0 = FALSE;
            }
            key = keyrow[column];

            switch (key) {
            case SHIFT_L: shift_l = make; break;
            case SHIFT_R: shift_r = make; break;
            case ALT_L:   alt_l   = make; break;
            case ALT_R:   alt_r   = make; break;
            case CTRL_L:  ctrl_l  = make; break;
            case CTRL_R:  ctrl_r  = make; break;
            default:                      break;
            }

            if (make) {
                key |= shift_l ? FLAG_SHIFT_L : 0;
                key |= shift_r ? FLAG_SHIFT_R : 0;
                key |= alt_l   ? FLAG_ALT_L   : 0;
                key |= alt_r   ? FLAG_ALT_R   : 0;
                key |= ctrl_l  ? FLAG_CTRL_L  : 0;
                key |= ctrl_r  ? FLAG_CTRL_R  : 0;

                in_process(p_tty, key);
            }
        }
    }
}

PRIVATE u8 get_byte_from_kbuf(void)
{
    u8 scan_code;

    while (kb_in.count <= 0) {}     /* Wait for new keypressed */

    disable_int();
    scan_code = *(kb_in.p_tail);
    kb_in.p_tail++;
    if (kb_in.p_tail == kb_in.buf + KB_IN_BYTES)
        kb_in.p_tail = kb_in.buf; 
    kb_in.count--;
    enable_int();

    return scan_code;
}