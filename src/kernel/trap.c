#include "type.h"
#include "const.h"
#include "proto.h"
#include "global.h"

PUBLIC void exception_handler(int vec_no, int err_code, int eip, int cs, int eflags)
{
    int text_color = 0x74; /* b:gray f:red */
    char * err_msg[] = {
        "#DE Divide Error",
        "#DB RESERVED",
        "--  NMI Interrupt",
        "#BP Breakpoint",
        "#OF Overflow",
        "#BR BOUND Range Exceeded",
        "#UD Invalid Opcode (Undefined Opcode)",
        "#NM Device Not Available (No Math Coprocessor)",
        "#DF Double Fault",
        "    Coprocessor Segment Overrun (reserved)",
        "#TS Invalid TSS",
        "#NP Segment Not Present",
        "#SS Stack-Segment Fault",
        "#GP General Protection",
        "#PF Page Fault",
        "--  (Intel reserved. Do not use.)",
        "#MF x87 FPU Floating-Point Error (Math Fault)",
        "#AC Alignment Check",
        "#MC Machine Check",
        "#XF SIMD Floating-Point Exception"
    };

    /* Clear screen */
    disp_pos = 0;
    for (int i = 0; i < 80 * 5; i++) disp_str(" ");
    disp_pos = 0;

    disp_color_str("Exception! --> ", text_color);
    disp_color_str(err_msg[vec_no], text_color);
    disp_color_str("\n\n", text_color);
    disp_color_str("EFLAGS:", text_color);
    disp_int(eflags);
    disp_color_str("CS:", text_color);
    disp_int(cs);
    disp_color_str("EIP:", text_color);
    disp_int(eip);

    if (err_code != 0xFFFFFFFF) {
        disp_color_str("Error Code:", text_color);
        disp_int(err_code);
    }
}