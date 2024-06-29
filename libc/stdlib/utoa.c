#include <stdlib.h>

char* utoa(unsigned int value, char* str, int base) {
    char* rc;
    char* ptr;
    char* low;
    // Set 'ptr' to point at the end of the string
    ptr = str;
    // Set 'rc' to point at the end of the string
    rc = ptr;
    // Handle the case of zero
    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return str;
    }
    // Convert the integer into a string
    while (value != 0) {
        *ptr++ = "0123456789abcdef"[value % base];
        value /= base;
    }
    // Terminate the string
    *ptr-- = '\0';
    // Reverse the string
    low = rc;
    while (low < ptr) {
        char temp = *low;
        *low++ = *ptr;
        *ptr-- = temp;
    }
    return str;
}