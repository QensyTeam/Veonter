#include <stdio.h>
#include <stdarg.h>  // For va_list and related functions
#include <stdint.h> // For uint8_t

#if __is_libk
#include <kernel/kernel.h>
#include "kernel/sys/pit.h"
#endif

#define COMMAND_BUFFER_SIZE 256

// Helper function to check if a character is whitespace
static int is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

// Helper function to skip whitespace in the input
static void skip_whitespace(const char** input) {
    while (is_whitespace(**input)) {
        (*input)++;
    }
}

int scanf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    char input_buffer[COMMAND_BUFFER_SIZE] = {0};
    size_t length = 0;
    char c;

    // Read the input line from the console
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

    while (*p) {
        // Skip any leading whitespace in the format string
        skip_whitespace(&p);

        if (*p == '%') {
            p++;
            skip_whitespace(&p);

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
                } else if (*p == 'f') {
                    double* double_arg = va_arg(args, double*);
                    *double_arg = strtod(input, (char**)&input);
                    matched++;
                }
            } else if (*p == 'f') {
                float* float_arg = va_arg(args, float*);
                *float_arg = strtof(input, (char**)&input);
                matched++;
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
                skip_whitespace(&input);
                while (*input && !is_whitespace(*input) && length < COMMAND_BUFFER_SIZE - 1) {
                    *str_arg++ = *input++;
                }
                *str_arg = '\0';
                matched++;
            } else if (*p == 'c') {
                char* char_arg = va_arg(args, char*);
                skip_whitespace(&input);
                *char_arg = *input++;
                matched++;
            }
            p++;
        } else {
            // Match literal characters in the format string
            skip_whitespace(&p);
            skip_whitespace(&input);
            if (*p != *input) {
                break;
            }
            p++;
            input++;
        }
    }

    va_end(args);
    return matched;
}
