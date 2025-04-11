#ifndef _ORANGES_VGA_H_
#define _ORANGES_VGA_H_

#define CRTC_ADDR_REG 0x3D4     /* CRT Controller Registers - Addr Register */
#define CRTC_DATA_REG 0x3D5     /* CRT Controller Registers - Data Register */
#define START_ADDR_H  0xC       /* Reg index of video mem start addr (MSB) */
#define START_ADDR_L  0xD       /* Reg index of video mem start addr (LSB) */
#define CURSOR_H      0xE       /* Reg index of cursor position (MSB) */
#define CURSOR_L      0xF       /* Reg index of cursor position (LSB) */
#define V_MEM_BASE    0xB8000   /* Base of color video memory */
#define V_MEM_SIZE    0x8000    /* 32K: B8000H -> BFFFFH */

#endif /* _ORANGES_VGA_H_ */