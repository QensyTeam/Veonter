#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <kernel/kernel.h>

void gtoa(double value, char* buffer, int precision) {
    if (precision < 0) precision = 6;

    char exp_buffer[96];
    etoa(value, exp_buffer, precision);

    ftoa(value, buffer, precision);
    
    // Сравниваем длины строк
    size_t len_regular = strlen(buffer);
    size_t len_exp = strlen(exp_buffer);

    if (len_exp < len_regular) {
        strcpy(buffer, exp_buffer);
    }

    char* dot_ptr = (char*)strchr(buffer, '.');
    if (dot_ptr) {
        char* end_ptr = buffer + strlen(buffer) - 1;
        while (end_ptr > dot_ptr && *end_ptr == '0') {
            *end_ptr-- = '\0';
        }
        if (*end_ptr == '.') {
            *end_ptr = '\0';
        }
    }
}
