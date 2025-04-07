#include "type.h"
#include "const.h"
#include "proto.h"
#include "global.h"
#include "protect.h"

PUBLIC char *itoa(char *buf, int num) 
{
    char *p = buf;

    *p++ = '0';
    *p++ = 'x';

    for (int i = 28; i >= 0; i-= 4) {
        char ch = (num >> i) & 0xF;
        ch += '0';
        if (ch > '9') ch += 7;
        *p++ = ch;
    }
    *p = '\0';

    return buf;
}

PUBLIC void disp_int(int input)
{
    char output[16];
    itoa(output, input);
    disp_str(output);
}