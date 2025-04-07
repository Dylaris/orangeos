/*
 * Stuff specifically for protect mode
 */

#ifndef _ORANGES_PROTECT_H_
#define _ORANGES_PROTECT_H_

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

/* IRQ entry point */
#define INT_VECTOR_IRQ0 0x20
#define INT_VECTOR_IRQ8 0x28

#endif /* _ORANGES_PROTECT_H_ */