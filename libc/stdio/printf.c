#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <kernel/drv/tty.h>

void putint(const size_t i) {
    char res[32];

    /*if (i < 0) {
        //terminal_putchar('-');
    }*/

    itoa(i, res, 0);
    puts(res);
}

int printf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    size_t count = 0;
    char buffer[32];

    while (*format) {
        if (*format == '%') {
            format++;
            if (*format == '%') {
                terminal_putchar('%');
                count++;
            } else {
                switch (*format) {
                    case 'd': // Вывод целого числа
                    {
                        int value = va_arg(args, int);
                        itoa(value, buffer, 10);
                        size_t len = strlen(buffer);
                        for (size_t i = 0; i < len; i++) {
                            terminal_putchar(buffer[i]);
                            count++;
                        }
                    }
                    break;
                    case 's': // Вывод строки
                    {
                        char* str = va_arg(args, char*);
                        size_t len = strlen(str);
                        for (size_t i = 0; i < len; i++) {
                            terminal_putchar(str[i]);
                            count++;
                        }
                    }
                    break;
                    case 'c': // Вывод символа
                    {
                        char c = va_arg(args, int);
                        terminal_putchar(c);
                        count++;
                    }
                    break;
                    default:
                        terminal_putchar('%');
                        terminal_putchar(*format);
                        count += 2;
                        break;
                }
            }
        } else {
            terminal_putchar(*format);
            count++;
        }
        format++;
    }

    va_end(args);
    return count;
}
