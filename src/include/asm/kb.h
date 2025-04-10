/*
 * Stuff about keyboard
 */

#ifndef _ORANGES_KB_H_
#define _ORANGES_KB_H_

/* 8042 ports */
#define KB_DATA 0x60 /* I/O port for keyboard data \
                      * Read : Read Output Buffer \
                      * Write: Write Input Buffer(8042 Data&8048 Command) */
#define KB_CMD  0x64 /* I/O port for keyboard command \
                      * Read : Read Status Register \
                      * Write: Write Input Buffer(8042 Command) */

#endif /* _ORANGES_KB_H_ */