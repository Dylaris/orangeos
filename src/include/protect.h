/*
 * Structure used in protect mode.
 */

#ifndef _ORANGES_PROTECT_H
#define _ORANGES_PROTECT_H

#include "type.h"

typedef struct s_descriptor {
    u16 limit_low;          /* Limit */
    u16 base_low;           /* Base */
    u8  base_mid;           /* Base */
    u8  attr1;              /* P(1) DPL(2) DT(1) TYPE(4) */
    u8  limit_high_attr2;   /* G(1) D(1) O(1) AVL(1) LimitHight(4) */
    u8  base_high;          /* Base */
} DESCRIPTOR; /* 8 bytes */

#endif /* _ORANGES_PROTECT_H */