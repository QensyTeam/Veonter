#include <stdlib.h>

void utoa(unsigned int value, char *buffer, int base) {
    //char temp[32];
    int i = 0;

    if (value == 0) {
        buffer[i++] = '0';
    } else {
        while (value != 0) {
            int rem = value % base;
            buffer[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
            value = value / base;
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
