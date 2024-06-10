#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void itoa(int value, char* str, int base) {
    // Handle 0 explicitly, otherwise empty string is printed
    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    // Negative numbers are handled only for base 10
    int is_negative = 0;
    if (value < 0 && base == 10) {
        is_negative = 1;
        value = -value;
    }

    // Process individual digits
    int i = 0;
    while (value != 0) {
        int rem = value % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        value = value / base;
    }

    // Add negative sign for base 10
    if (is_negative && base == 10) {
        str[i++] = '-';
    }
    str[i] = '\0';

    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}