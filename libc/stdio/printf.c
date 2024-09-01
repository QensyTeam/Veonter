#include "kernel/sys/gui/shell.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <kernel/kernel.h>

// Объявляем функции преобразования для поддержки разных типов
void putint(const size_t i) {
    char res[32];
    itoa(i, res, 0);
    puts(res);
}

// Обновленная функция printf
int printf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    size_t count = 0;
    char buffer[96] = {0};  // Увеличен размер буфера для больших чисел и строк

    while (*format) {
        if (*format == '%') {
            format++;
            if (*format == '%') {
                shell_putchar('%');
                count++;
            } else {
                int width = 0;          // Ширина вывода (например, 08 для %08llx)
                int precision = -1;     // Точность для чисел с плавающей запятой (например, .6 для %.6f)
                char pad_char = ' ';    // Символ заполнения (по умолчанию пробел)

                // Обрабатываем опции формата
                if (*format == '0') {
                    pad_char = '0'; // Если указано %0, то использовать '0' для заполнения
                    format++;
                }
                
                // Чтение ширины поля
                while (*format >= '0' && *format <= '9') {
                    width = width * 10 + (*format - '0');
                    format++;
                }

                // Чтение точности
                if (*format == '.') {
                    format++;
                    precision = 0;
                    while (*format >= '0' && *format <= '9') {
                        precision = precision * 10 + (*format - '0');
                        format++;
                    }
                }

                switch (*format) {
                    case 'd': // Вывод целого числа
                    {
                        int value = va_arg(args, int);
                        itoa(value, buffer, 10);
                        size_t len = strlen(buffer);
                        for (size_t i = 0; i < len; i++) {
                            shell_putchar(buffer[i]);
                            count++;
                        }
                    }
                    break;
                    case 'u': // Вывод unsigned int
                    {
                        unsigned int value = va_arg(args, unsigned int);
                        utoa(value, buffer, 10);
                        size_t len = strlen(buffer);
                        for (size_t i = 0; i < len; i++) {
                            shell_putchar(buffer[i]);
                            count++;
                        }
                    }
                    break;
                    case 's': // Вывод строки
                    {
                        uint8_t* str = va_arg(args, uint8_t*);
                        
                        while(*str) {
                            uint16_t ch = (uint16_t)*str;

                            if(ch == 0xd0 || ch == 0xd1) {
                                ch |= (*++str) << 8;
                                count++;
                            }

                            shell_putchar(ch);
                            str++;
                            count++;
                        }
                    }
                    break;
                    case 'c': // Вывод символа
                    {
                        char c = va_arg(args, int);
                        shell_putchar(c);
                        count++;
                    }
                    break;
                    case 'p': // Вывод указателя
                    {
                        void* ptr = va_arg(args, void*);
                        itoa((size_t)ptr, buffer, 16); // Преобразуем указатель в строку в шестнадцатеричном формате
                        size_t len = strlen(buffer);
                        for (size_t i = 0; i < len; i++) {
                            shell_putchar(buffer[i]);
                            count++;
                        }
                    }
                    break;
                    case 'x': // Вывод целого числа в шестнадцатеричном формате (нижний регистр)
                    {
                        int value = va_arg(args, int);
                        itoa(value, buffer, 16);
                        size_t len = strlen(buffer);
                        for (size_t i = 0; i < len; i++) {
                            shell_putchar(buffer[i]);
                            count++;
                        }
                    }
                    break;
                    case 'X': // Вывод целого числа в шестнадцатеричном формате (верхний регистр)
                    {
                        int value = va_arg(args, int);
                        itoa(value, buffer, 16);
                        for (size_t i = 0; i < strlen(buffer); i++) {
                            shell_putchar(toupper(buffer[i])); // Преобразование в верхний регистр
                            count++;
                        }
                    }
                    break;
                    case 'o': // Вывод целого числа в восьмеричном формате
                    {
                        unsigned int value = va_arg(args, unsigned int);
                        utoa(value, buffer, 8); // Преобразуем число в строку в восьмеричном формате
                        size_t len = strlen(buffer);
                        for (size_t i = 0; i < len; i++) {
                            shell_putchar(buffer[i]);
                            count++;
                        }
                    }
                    break;
                    case 'b': // Вывод целого числа в двоичном формате
                    {
                        unsigned int value = va_arg(args, unsigned int);
                        utoa(value, buffer, 2); // Преобразуем число в строку в двоичном формате
                        size_t len = strlen(buffer);
                        for (size_t i = 0; i < len; i++) {
                            shell_putchar(buffer[i]);
                            count++;
                        }
                    }
                    break;
                    case 'n': // Сохранение количества выведенных символов
                    {
                        int* pcount = va_arg(args, int*);
                        *pcount = count;
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
                                shell_putchar(buffer[i]);
                                count++;
                            }
                        } else {
                            shell_putchar('%');
                            shell_putchar('z');
                            shell_putchar(*format);
                            count += 3;
                        }
                    }
                    break;
                    case 'f': // Вывод числа с плавающей запятой
                    {
                        double value = va_arg(args, double);
                        int prec = (precision == -1) ? 6 : precision; // Используем точность по умолчанию 6, если не указана
                        ftoa(value, buffer, prec);  // Используем указанную точность
                        size_t len = strlen(buffer);
                        for (size_t i = 0; i < len; i++) {
                            shell_putchar(buffer[i]);
                            count++;
                        }
                    }
                    break;
                    case 'e': // Вывод числа в экспоненциальном формате
                    {
                        double value = va_arg(args, double);
                        int prec = (precision == -1) ? 6 : precision; // Используем точность по умолчанию 6, если не указана
                        etoa(value, buffer, prec);  // Используем указанную точность
                        size_t len = strlen(buffer);
                        for (size_t i = 0; i < len; i++) {
                            shell_putchar(buffer[i]);
                            count++;
                        }
                    }
                    break;
                    case 'g': // Вывод числа в кратчайшем формате
                    {
                        double value = va_arg(args, double);
                        int prec = (precision == -1) ? 6 : precision; // Используем точность по умолчанию 6, если не указана
                        gtoa(value, buffer, prec);  // Используем указанную точность
                        size_t len = strlen(buffer);
                        for (size_t i = 0; i < len; i++) {
                            shell_putchar(buffer[i]);
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
                                shell_putchar(buffer[i]);
                                count++;
                            }
                        } else if (*format == 'l') { // long long
                            format++;
                            if (*format == 'd') {
                                long long value = va_arg(args, long long);
                                lltoa(value, buffer, 10);
                                size_t len = strlen(buffer);
                                for (size_t i = 0; i < len; i++) {
                                    shell_putchar(buffer[i]);
                                    count++;
                                }
                            } else if(*format == 'u') { // %llu - unsigned long long
                                unsigned long long value = va_arg(args, unsigned long long);
                                llutoa(value, buffer, 10);
                                size_t len = strlen(buffer);
                                for (size_t i = 0; i < len; i++) {
                                    shell_putchar(buffer[i]);
                                    count++;
                                }
                            } else if (*format == 'x') { // %llx - long long в шестнадцатеричном формате
                                unsigned long long value = va_arg(args, unsigned long long);
                                llutoa(value, buffer, 16);
                                size_t len = strlen(buffer);
                                
                                // Для формата %08llx добавляем ведущие нули
                                if (width > len) {
                                    for (size_t i = 0; i < width - len; i++) {
                                        shell_putchar(pad_char);
                                        count++;
                                    }
                                }

                                for (size_t i = 0; i < len; i++) {
                                    shell_putchar(buffer[i]);
                                    count++;
                                }
                            } else {
                                shell_putchar('%');
                                shell_putchar('l');
                                shell_putchar('l');
                                shell_putchar(*format);
                                count += 4;
                            }
                        } else if (*format == 'f') { // long double
                            long double value = va_arg(args, long double);
                            int prec = (precision == -1) ? 6 : precision; // Используем точность по умолчанию 6, если не указана
                            lftoa(value, buffer, prec);  // Используем указанную точность
                            size_t len = strlen(buffer);
                            for (size_t i = 0; i < len; i++) {
                                shell_putchar(buffer[i]);
                                count++;
                            }
                        } else if(*format == 'u') { // %lu - unsigned long
                            unsigned long value = va_arg(args, unsigned long);
                            lutoa(value, buffer, 10);
                            size_t len = strlen(buffer);
                            for (size_t i = 0; i < len; i++) {
                                shell_putchar(buffer[i]);
                                count++;
                            }
                        } else if (*format == 'x') { // %lx - unsigned long в шестнадцатеричном формате
                            unsigned long value = va_arg(args, unsigned long);
                            lutoa(value, buffer, 16);
                            size_t len = strlen(buffer);
                            for (size_t i = 0; i < len; i++) {
                                shell_putchar(buffer[i]);
                                count++;
                            }
                        } else {
                            shell_putchar('%');
                            shell_putchar('l');
                            shell_putchar(*format);
                            count += 3;
                        }
                    }
                    break;
                    case 'L': // Вывод long double
                    {
                        format++;
                        if (*format == 'f') {
                            long double value = va_arg(args, long double);
                            int prec = (precision == -1) ? 6 : precision; // Используем точность по умолчанию 6, если не указана
                            lftoa(value, buffer, prec);  // Используем указанную точность
                            size_t len = strlen(buffer);
                            for (size_t i = 0; i < len; i++) {
                                shell_putchar(buffer[i]);
                                count++;
                            }
                        } else {
                            shell_putchar('%');
                            shell_putchar('L');
                            shell_putchar(*format);
                            count += 3;
                        }
                    }
                    break;
                    default:
                        shell_putchar('%');
                        shell_putchar(*format);
                        count += 2;
                        break;
                }
            }
        } else {
            uint16_t ch = (uint16_t)(*(uint8_t*)format);

            if(ch == 0xd0 || ch == 0xd1) {
                ch |= (*(uint8_t*)(++format)) << 8;
                count++;
            }

            shell_putchar(ch);
            count++;
        }
        format++;
    }

    va_end(args);
    return count;
}
