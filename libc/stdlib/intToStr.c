#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <kernel/kernel.h>

int intToStr(int x, char str[], int d) {
    int i = 0;
    if (x == 0) {
        str[i++] = '0';
    } else {
        while (x != 0) {
            str[i++] = (x % 10) + '0';
            x = x / 10;
        }
    }

    while (i < d) {
        str[i++] = '0';
    }

    reverse(str, i);
    str[i] = '\0';
    return i;
}