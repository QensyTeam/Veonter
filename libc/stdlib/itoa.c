#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// Преобразование целых чисел в строки
void itoa(int value, char *buffer, int base) {
    char temp[32];
    int i = 0;
    int is_negative = 0;

    if (value == 0) {
        buffer[i++] = '0';
    } else {
        if (value < 0 && base == 10) {
            is_negative = 1;
            value = -value;
        }

        while (value != 0) {
            int rem = value % base;
            buffer[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
            value = value / base;
        }

        if (is_negative) {
            buffer[i++] = '-';
        }
        buffer[i] = '\0';

        // Reverse the string
        int start = 0;
        int end = i - 1;
        while (start < end) {
            char temp_char = buffer[start];
            buffer[start] = buffer[end];
            buffer[end] = temp_char;
            start++;
            end--;
        }
    }
}