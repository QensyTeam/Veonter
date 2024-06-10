#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <kernel/drv/tty.h>

void putint(const size_t i) {
    char res[32];
    itoa(i, res, 0);
    puts(res);
}




int printf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    size_t count = 0;
    char buffer[64];  // Увеличен размер буфера для больших чисел и строк

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
                    case 'p': // Вывод указателя
                    {
                        void* ptr = va_arg(args, void*);
                        itoa((size_t)ptr, buffer, 16); // Преобразуем указатель в строку в шестнадцатеричном формате
                        size_t len = strlen(buffer);
                        for (size_t i = 0; i < len; i++) {
                            terminal_putchar(buffer[i]);
                            count++;
                        }
                    }
                    break;
                    case 'x': // Вывод целого числа в шестнадцатеричном формате
                    {
                        int value = va_arg(args, int);
                        itoa(value, buffer, 16);
                        size_t len = strlen(buffer);
                        for (size_t i = 0; i < len; i++) {
                            terminal_putchar(buffer[i]);
                            count++;
                        }
                    }
                    break;
                    case 'z': // Вывод size_t
                    {
                        format++;
                        if (*format == 'u') {
                            size_t value = va_arg(args, size_t);
                            itoa(value, buffer, 10);
                            size_t len = strlen(buffer);
                            for (size_t i = 0; i < len; i++) {
                                terminal_putchar(buffer[i]);
                                count++;
                            }
                        } else {
                            terminal_putchar('%');
                            terminal_putchar('z');
                            terminal_putchar(*format);
                            count += 3;
                        }
                    }
                    break;
                    case 'f': // Вывод числа с плавающей запятой
                    {
                        double value = va_arg(args, double);
                        ftoa(value, buffer, 6);  // Используем точность 6 цифр после запятой
                        size_t len = strlen(buffer);
                        for (size_t i = 0; i < len; i++) {
                            terminal_putchar(buffer[i]);
                            count++;
                        }
                    }
                    break;
                    case 'l': // Вывод long и long long
                    {
                        format++;
                        if (*format == 'd') { // long
                            long value = va_arg(args, long);
                            ltoa(value, buffer, 10);
                            size_t len = strlen(buffer);
                            for (size_t i = 0; i < len; i++) {
                                terminal_putchar(buffer[i]);
                                count++;
                            }
                        } else if (*format == 'l') { // long long
                            format++;
                            if (*format == 'd') {
                                long long value = va_arg(args, long long);
                                lltoa(value, buffer, 10);
                                size_t len = strlen(buffer);
                                for (size_t i = 0; i < len; i++) {
                                    terminal_putchar(buffer[i]);
                                    count++;
                                }
                            } else {
                                terminal_putchar('%');
                                terminal_putchar('l');
                                terminal_putchar('l');
                                terminal_putchar(*format);
                                count += 4;
                            }
                        } else if (*format == 'f') { // long double
                            long double value = va_arg(args, long double);
                            lftoa(value, buffer, 6);  // Используем точность 6 цифр после запятой
                            size_t len = strlen(buffer);
                            for (size_t i = 0; i < len; i++) {
                                terminal_putchar(buffer[i]);
                                count++;
                            }
                        } else {
                            terminal_putchar('%');
                            terminal_putchar('l');
                            terminal_putchar(*format);
                            count += 3;
                        }
                    }
                    break;
                    case 'L': // Вывод long double
                    {
                        format++;
                        if (*format == 'f') {
                            long double value = va_arg(args, long double);
                            lftoa(value, buffer, 6);  // Используем точность 6 цифр после запятой
                            size_t len = strlen(buffer);
                            for (size_t i = 0; i < len; i++) {
                                terminal_putchar(buffer[i]);
                                count++;
                            }
                        } else {
                            terminal_putchar('%');
                            terminal_putchar('L');
                            terminal_putchar(*format);
                            count += 3;
                        }
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
