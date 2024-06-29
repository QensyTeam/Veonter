#include <stdlib.h>

void lltoa(long long value, char *str, int base) {
    char *ptr = str, *ptr1 = str, tmp_char;
    long long tmp_value;
    int is_negative = 0;

    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "0123456789abcdef"[tmp_value - value * base];
    } while (value);

    if (is_negative) *ptr++ = '-';
    *ptr-- = '\0';
    
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}