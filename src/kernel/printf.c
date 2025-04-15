#include "sys/const.h"
#include "sys/proto.h"
#include "type.h"
#include "string.h"

PUBLIC int printf(const char *fmt, ...)
{
    char buf[256];

    va_list arg = (va_list) ((char *) &fmt + 4);
    int len = vsprintf(buf, fmt, arg);
    buf[len] = '\0';
    printx(buf);

    return len;
}

PUBLIC int vsprintf(char *buf, const char *fmt, va_list args)
{
    char *p;
    char tmp[256];
    va_list p_next_arg = args;

    for (p = buf; *fmt; fmt++) {
        if (*fmt != '%') {
            *p++ = *fmt;
            continue;
        }

        fmt++;

        switch (*fmt) {
        case 'x':
            itoa(tmp, *((int *) p_next_arg));
            strcpy(p, tmp);
            p_next_arg += 4;
            p += strlen(tmp);
            break;

        case 's':
            break;

        default:
            break;
        }
    }

    return p - buf;
}