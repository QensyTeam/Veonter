#include <stdlib.h>
#include <string.h>

void ftoa(double value, char *str, int precision) {
    int is_negative = 0;
    if (value < 0) {
        is_negative = 1;
        value = -value;
    }
    
    int int_part = (int)value;
    double frac_part = value - int_part;
    char *ptr = str;

    if (is_negative) {
        *ptr++ = '-';
    }

    ltoa(int_part, ptr, 10);
    ptr += strlen(ptr);

    if (precision > 0) {
        *ptr++ = '.';
        while (precision--) {
            frac_part *= 10.0;
            int frac_digit = (int)frac_part;
            *ptr++ = '0' + frac_digit;
            frac_part -= frac_digit;
        }
    }

    *ptr = '\0';
}