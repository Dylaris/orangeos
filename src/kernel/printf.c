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

#if 0
/**
 * @brief Convert to integer to string with given base
 * @param num  Integer value
 * @param base Convert base
 * @param buf  Buffer to store result string
 * @return Next position of buffer
 */
PRIVATE char *i2a(int num, int base, char **buf)
{
    char digits[] = "0123456789ABCDEF";
    char tmp[STR_DEFAULT_LEN];
    int pos = 0; /* Buffer index */

    if (num == 0) {
        (*buf)[pos++] = '0';
    } else {
        /* The number string in tmp is reversed order */
        while (num) {
            tmp[pos++] = digits[num % base];
            num /= base;
        }
    }

    /* Reverse the string in tmp to buf */
    for (int i = pos - 1; i >= 0; i--) {
        (*buf)[pos - 1 - i] = tmp[i];
    }

    *buf += pos;

    return *buf;
}

/**
 * @brief Write the fortmat string filled with args to buffer
 * @param buf Result buffer
 * @param fmt Output format
 * @param args Variable arguments
 * @return Length of string
 */
PUBLIC int vsprintf(char *buf, const char *fmt, va_list args)
{
    char *p_buf = buf;
    va_list p_next_arg = args;
    int align_nr = 0;   /* Allignment number */
    char tmp_buf[STR_DEFAULT_LEN];
    char pad_char = ' ';

    while (*fmt) {
        /* Copy directly if it is an unformatted character */
        if (*fmt != '%') {
            *p_buf++ = *fmt;
            continue;
        }
        fmt++; /* Skip '%' */

        /* Update padding character */
        if (*fmt == '0') {
            pad_char = '0';
            fmt++; /* Skip '0' */
        }

        /* Process alignment number */
        align_nr = 0;
        while (*fmt >= '0' && *fmt <= '9') {
            align_nr = align_nr * 10 + (*fmt - '0');
            fmt++;
        }

        /* Process formatted character */
        char *p_tmp = tmp_buf;
        memset(p_tmp, 0, sizeof(tmp_buf));
        switch (*fmt) {
        case 'c': {
            char ch = *((char *) p_next_arg);
            *p_tmp = ch;
            p_next_arg += 4;
            break;
        }
        case 'x': {
            int num = *((int *) p_next_arg);
            i2a(num, 16, &p_tmp);
            p_next_arg += 4;
            break;
        }
        case 'd': {
            int num = *((int *) p_next_arg);
            if (num < 0) {
                *p_tmp++ = '-';
                num = -num;
            }
            i2a(num, 10, &p_tmp);
            p_next_arg += 4;
            break;
        }
        case 's': {
            char *str = *((char **) p_next_arg);
            strcpy(p_tmp, str);
            p_next_arg += 4;  
            break;
        }
        default:
            break;
        }

        int padding = (align_nr > strlen(tmp_buf)) ? (align_nr - strlen(tmp_buf)) : 0;
        for (int i = 0; i < padding; i++) {
            *p_buf++ = pad_char;
        }

        p_tmp = tmp_buf;
        while (*p_tmp) {
            *p_buf++ = *p_tmp++;
        }

        fmt++;  /* Next character */
    }

    *p_buf = '\0';
    return p_buf - buf;
}
#endif

/*======================================================================*
                                i2a
 *======================================================================*/
PRIVATE char* i2a(int val, int base, char ** ps)
{
	int m = val % base;
	int q = val / base;
	if (q) {
		i2a(q, base, ps);
	}
	*(*ps)++ = (m < 10) ? (m + '0') : (m - 10 + 'A');

	return *ps;
}


/*======================================================================*
                                vsprintf
 *======================================================================*/
/*
 *  为更好地理解此函数的原理，可参考 printf 的注释部分。
 */
PUBLIC int vsprintf(char *buf, const char *fmt, va_list args)
{
	char*	p;

	va_list	p_next_arg = args;
	int	m;

	char	inner_buf[STR_DEFAULT_LEN];
	char	cs;
	int	align_nr;

	for (p=buf;*fmt;fmt++) {
		if (*fmt != '%') {
			*p++ = *fmt;
			continue;
		}
		else {		/* a format string begins */
			align_nr = 0;
		}

		fmt++;

		if (*fmt == '%') {
			*p++ = *fmt;
			continue;
		}
		else if (*fmt == '0') {
			cs = '0';
			fmt++;
		}
		else {
			cs = ' ';
		}
		while (((unsigned char)(*fmt) >= '0') && ((unsigned char)(*fmt) <= '9')) {
			align_nr *= 10;
			align_nr += *fmt - '0';
			fmt++;
		}

		char * q = inner_buf;
		memset(q, 0, sizeof(inner_buf));

		switch (*fmt) {
		case 'c':
			*q++ = *((char*)p_next_arg);
			p_next_arg += 4;
			break;
		case 'x':
			m = *((int*)p_next_arg);
			i2a(m, 16, &q);
			p_next_arg += 4;
			break;
		case 'd':
			m = *((int*)p_next_arg);
			if (m < 0) {
				m = m * (-1);
				*q++ = '-';
			}
			i2a(m, 10, &q);
			p_next_arg += 4;
			break;
		case 's':
			strcpy(q, (*((char**)p_next_arg)));
			q += strlen(*((char**)p_next_arg));
			p_next_arg += 4;
			break;
		default:
			break;
		}

		int k;
		for (k = 0; k < ((align_nr > strlen(inner_buf)) ? (align_nr - strlen(inner_buf)) : 0); k++) {
			*p++ = cs;
		}
		q = inner_buf;
		while (*q) {
			*p++ = *q++;
		}
	}

	*p = 0;

	return (p - buf);
}


/*======================================================================*
                                 sprintf
 *======================================================================*/
int sprintf(char *buf, const char *fmt, ...)
{
	va_list arg = (va_list)((char*)(&fmt) + 4);        /* 4 是参数 fmt 所占堆栈中的大小 */
	return vsprintf(buf, fmt, arg);
}
