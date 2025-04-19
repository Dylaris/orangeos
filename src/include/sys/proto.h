/*
 * Function prototype
 */

#ifndef _ORANGES_PROTO_H_
#define _ORANGES_PROTO_H_

#include "sys/const.h"
#include "sys/console.h"
#include "sys/tty.h"
#include "sys/proc.h"
#include "type.h"

/* Virtual address to physics address */
#define va2pa(seg_base, va) ((u32) (seg_base) + (u32) (va))

/* kernel/asm/pic.c */
PUBLIC void init_pic(void);
PUBLIC void put_irq_handler(int irq, irq_handler handler);

/* kernel/asm/pit.c */
PUBLIC void init_pit(void);

/* kernel/dev/clock.c */
PUBLIC void init_clock(void);
PUBLIC void clock_handler(int irq);
PUBLIC void milli_delay(int time);

/* kernel/dev/keyboard.c */
PUBLIC void init_keyboard(void);
PUBLIC void keyboard_read(TTY *p_tty);
PUBLIC void keyboard_handler(int irq);

/* kernel/dev/hd.c */
PUBLIC void task_hd(void);
PUBLIC void hd_handler(int irq);

/* kernel/init.c */
PUBLIC void init(void);

/* kernel/kernel.asm */
PUBLIC void restart(void);      /* Restore context and then restart execution */

/* kernel/main.c */
PUBLIC int kernel_main(void);
PUBLIC void panic(const char *fmt, ...);
PUBLIC int get_ticks(void);
PUBLIC void TestA(void);
PUBLIC void TestB(void);
PUBLIC void TestC(void);

/* kernel/proc.c */
PUBLIC void schedule(void);
PUBLIC void *va2la(int pid, void *va);
PUBLIC int ldt_seg_linear(PROCESS *p_proc, int idx);
PUBLIC void reset_msg(MESSAGE *p_msg);
PUBLIC int send_recv(int funciton, int src_dest, MESSAGE *p_msg);
PUBLIC void inform_int(int task_nr);

/* kernel/tty.c */
PUBLIC void task_tty(void);
PUBLIC void in_process(TTY *p_tty, u32 key);
PUBLIC void tty_write(TTY *p_tty, char *buf, int len);

/* kernel/console.c */
PUBLIC int is_current_console(CONSOLE *p_con);
PUBLIC void out_char(CONSOLE *p_con, char ch);
PUBLIC void init_screen(TTY *p_tty);
PUBLIC void select_console(int nr_console);
PUBLIC void scroll_screen(CONSOLE *p_con, int direction);

/* kernel/trap.c */
PUBLIC void exception_handler(int vec_no, int err_code, int eip, int cs, int eflags);
PUBLIC void spurious_irq(int irq);

/* kernel/printf.c */
PUBLIC int printf(const char *fmt, ...);
#define printl printf
PUBLIC int vsprintf(char *buf, const char *fmt, va_list args);
PUBLIC int sprintf(char *buf, const char *fmt, ...);

/* kernel/syscall.c */
PUBLIC void task_sys(void);

/* lib/klib.c */
PUBLIC char *itoa(char *buf, int num);
PUBLIC void disp_int(int input);

/* lib/misc.c */
PUBLIC void spin(char *func_name);

/* lib/kliba.asm */
PUBLIC void out_byte(u16 port, u8 value);
PUBLIC u8   in_byte(u16 port);
PUBLIC void disp_str(char *str);
PUBLIC void disp_color_str(char *str, int color);
PUBLIC void disable_irq(int irq);
PUBLIC void enable_irq(int irq);
PUBLIC void disable_int(void);
PUBLIC void enable_int(void);
PUBLIC void port_read(u16 port, void *buf, int n);
PUBLIC void port_write(u16 port, void *buf, int n);

/* System call handler */
PUBLIC void sys_call(void);

/* System call - user space */
PUBLIC int printx(char *str);
PUBLIC int sendrec(int function, int send_recv, MESSAGE *p_msg);

/* System call - kernel space */
PUBLIC int sys_printx(int _unused1, int _unused2, char *str, PROCESS *p_proc);
PUBLIC int sys_sendrec(int function, int send_recv, MESSAGE *p_msg, PROCESS *p_proc);

#endif /* _ORANGES_PROTO_H_ */