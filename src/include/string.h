#ifndef _ORANGES_STRING_H_
#define _ORANGES_STRING_H_

#include "sys/const.h"

PUBLIC void *memcpy(void *dst, void *src, int size);
PUBLIC void memset(void* dst, char ch, int size);
PUBLIC char *strcpy(char *dst, char *src);
PUBLIC int strlen(char *str);

#endif /* _ORANGES_STRING_H_ */