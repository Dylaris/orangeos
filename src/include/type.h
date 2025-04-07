/*
 * Type
 */

#ifndef _ORANGES_TYPE_H_
#define _ORANGES_TYPE_H_

typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char  u8;

typedef struct s_descriptor {
	u16 limit_low;
	u16 base_low;
	u8  base_mid;
	u8  attr1;
	u8  limit_high_attr2;
	u8  base_high;
} DESCRIPTOR;

typedef struct s_gate {
	u16 offset_low;
	u16 selector;
	u8 	dcount; /* Count of ouble word arg needed to be copyed in call gate */
	u8  attr;
	u16 offset_high;
} GATE;

typedef void (*int_handler)(void);

#endif /* _ORANGES_TYPE_H_ */