#include "type.h"
#include "const.h"
#include "proto.h"
#include "global.h"

PUBLIC char *itoa(char *buf, int num) 
{
    char *p = buf;
    int flag = 0;

    *p++ = '0';
    *p++ = 'x';

    if (num == 0) {
        *p++ = '0';
    } else {
        for (int i = 28; i >= 0; i-= 4) {
            char ch = (num >> i) & 0xF;
            if (flag || ch > 0) {
                flag = 1;
                ch += '0';
                if (ch > '9') ch += 7;
                *p++ = ch;
            }
        }
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

PUBLIC void delay(int time)
{
    for (int k = 0; k < time; k++) {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10000; j++) {}
        }
    }
}
