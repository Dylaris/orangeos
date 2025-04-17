#ifndef _ORANGES_STRING_H_
#define _ORANGES_STRING_H_

#include "sys/const.h"

PUBLIC void *memcpy(void *dst, void *src, int size);
PUBLIC void memset(void* dst, char ch, int size);
PUBLIC char *strcpy(char *dst, char *src);
PUBLIC int  strlen(char *str);

/**
 * `phys_copy' and `phys_set' are used only in the kernel, where segments
 * are all flat (based on 0). In the meanwhile, currently linear address
 * space is mapped to the identical physical address space. Therefore,
 * a `physical copy' will be as same as a common copy, so does `phys_set'.
 */
#define phys_copy memcpy
#define phys_set  memset

#endif /* _ORANGES_STRING_H_ */