#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <kernel/kernel.h>

void etoa(double value, char* buffer, int precision) {
    if (precision < 0) precision = 6;

    int exp = 0;
    bool negative = false;

    if (value < 0) {
        negative = true;
        value = -value;
    }

    if (value != 0.0) {
        while (value >= 10.0) {
            value /= 10.0;
            exp++;
        }
        while (value < 1.0) {
            value *= 10.0;
            exp--;
        }
    }

    if (negative) {
        *buffer++ = '-';
    }

    ftoa(value, buffer, precision);

    while (*buffer != '\0') buffer++;
    *buffer++ = 'e';

    if (exp < 0) {
        *buffer++ = '-';
        exp = -exp;
    } else {
        *buffer++ = '+';
    }

    intToStr(exp, buffer, 2);
}