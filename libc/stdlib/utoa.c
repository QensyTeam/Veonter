#include <stdlib.h>

char* utoa(unsigned int value, char* str, int base) {
    char* rc;
    char* ptr;
    char* low;
    ptr = str;
    rc = ptr;
    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return str;
    }
    while (value != 0) {
        *ptr++ = "0123456789abcdef"[value % base];
        value /= base;
    }
    *ptr-- = '\0';
    low = rc;
    while (low < ptr) {
        char temp = *low;
        *low++ = *ptr;
        *ptr-- = temp;
    }
    return str;
}