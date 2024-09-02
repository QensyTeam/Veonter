#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <kernel/kernel.h>

void gtoa(double value, char *buffer, int precision) {
    char f_buffer[64];
    char e_buffer[64];

    ftoa(value, f_buffer, precision);
    etoa(value, e_buffer, precision);

    if (strlen(f_buffer) <= strlen(e_buffer)) {
        strcpy(buffer, f_buffer);
    } else {
        strcpy(buffer, e_buffer);
    }
}