#include "kernel/sys/pit.h"
#include <stdio.h>
#include <kernel/kernel.h>
#include <stdarg.h>  // Для va_list и связанных функций

#define COMMAND_BUFFER_SIZE 256

int scanf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    char input_buffer[COMMAND_BUFFER_SIZE];
    size_t length = 0;
    char c;

    // Считывание строки с консоли
    while (1) {
        c = keyboard_get_char();
        
        if (c != '\b') {
            putchar(c);
        }

        if (c == '\n' || c == '\r') {
            if (length < COMMAND_BUFFER_SIZE) {
                input_buffer[length] = '\0';
                break;
            }
        } else if (c == '\b') {
            if (length > 0) {
                length--;
                putchar('\b');
            }
        } else {
            if (length < COMMAND_BUFFER_SIZE - 1) {
                input_buffer[length++] = c;
            }
        }
    }

    const char* p = format;
    const char* input = input_buffer;
    int matched = 0;

    while (*p && *input) {
        if (*p == '%') {
            p++;
            if (*p == 'h') {
                p++;
                if (*p == 'h' && *(p + 1) == 'd') {
                    char* char_arg = va_arg(args, char*);
                    *char_arg = (char)strtol(input, (char**)&input, 10);
                    matched++;
                    p++;
                } else if (*p == 'd') {
                    short* short_arg = va_arg(args, short*);
                    *short_arg = (short)strtol(input, (char**)&input, 10);
                    matched++;
                }
            } else if (*p == 'l') {
                p++;
                if (*p == 'd') {
                    long* long_arg = va_arg(args, long*);
                    *long_arg = strtol(input, (char**)&input, 10);
                    matched++;
                } else if (*p == 'l' && *(p + 1) == 'd') {
                    long long* long_long_arg = va_arg(args, long long*);
                    *long_long_arg = strtoll(input, (char**)&input, 10);
                    matched++;
                    p++;
                }
            } else if (*p == 'f') {
                float* float_arg = va_arg(args, float*);
                *float_arg = strtof(input, (char**)&input);
                matched++;
            } else if (*p == 'l' && *(p + 1) == 'f') {
                double* double_arg = va_arg(args, double*);
                *double_arg = strtod(input, (char**)&input);
                matched++;
                p++;
            } else if (*p == 'L' && *(p + 1) == 'f') {
                long double* long_double_arg = va_arg(args, long double*);
                *long_double_arg = strtold(input, (char**)&input);
                matched++;
                p++;
            } else if (*p == 'd') {
                int* int_arg = va_arg(args, int*);
                *int_arg = (int)strtol(input, (char**)&input, 10);
                matched++;
            } else if (*p == 's') {
                char* str_arg = va_arg(args, char*);
                while (*input && *input != ' ') {
                    *str_arg++ = *input++;
                }
                *str_arg = '\0';
                matched++;
            } else if (*p == 'c') {
                char* char_arg = va_arg(args, char*);
                *char_arg = *input++;
                matched++;
            }
        } else {
            if (*p == *input) {
                input++;
            }
        }
        p++;
    }

    va_end(args);
    return matched;
}
