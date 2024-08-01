#include <string.h>

long double strtold(const char *str, char **endptr) {
    long double result = 0.0;
    int sign = 1;
    long double factor = 1.0;

    // Пропустить начальные пробелы
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r' || *str == '\f' || *str == '\v') {
        str++;
    }

    // Проверка знака числа
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // Преобразование целой части
    while ((*str >= '0' && *str <= '9')) {
        result = result * 10.0 + (*str - '0');
        str++;
    }

    // Преобразование дробной части
    if (*str == '.') {
        str++;
        while ((*str >= '0' && *str <= '9')) {
            factor /= 10.0;
            result += (*str - '0') * factor;
            str++;
        }
    }

    if (endptr) {
        *endptr = (char *)str;
    }

    return result * sign;
}