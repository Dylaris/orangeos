/*
 * Common constant
 */

#ifndef _ORANGES_CONST_H_
#define _ORANGES_CONST_H_

#define PUBLIC
#define PRIVATE static
#define EXTERN  extern

#define TRUE    1
#define FALSE   0

#define NULL ((void *) 0)

/*
 * Color
 *
 * e.g. MAKE_COLOR(BLUE, RED)
 *      MAKE_COLOR(BLACK, RED) | BRIGHT
 *      MAKE_COLOR(BLACK, RED) | BRIGHT | FLASH
 */
#define BLACK   0x0     /* 0000 */
#define WHITE   0x7     /* 0111 */
#define RED     0x4     /* 0100 */
#define GREEN   0x2     /* 0010 */
#define BLUE    0x1     /* 0001 */
#define FLASH   0x80    /* 1000 0000 */
#define BRIGHT  0x08    /* 0000 1000 */

#define MAKE_COLOR(b, f) ((b<<4) | f) /* MAKE_COLOR(Background, Foreground) */

#define NR_SYS_CALL 2   /* Number of system call */
#define NR_TASKS    2   /* Number of tasks */
#define NR_PROCS    3   /* Number of processes */
#define NR_CONSOLES 3   /* Number of consoles */

/* Magic chars used by `printx' */
#define MAG_CH_PANIC    '\002'
#define MAG_CH_ASSERT   '\003'

#define ASSERT
#ifdef ASSERT
PUBLIC void assertion_failure(char *exp, char *file, char *base_file, int line);
#define assert(exp) if (exp) ; \
                    else assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__);
#else
#define assert(exp)
#endif /* ASSERT */

#define STR_DEFAULT_LEN 1024

#endif /* _ORANGES_CONST_H_ */