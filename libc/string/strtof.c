#include <string.h>

float strtof(const char *str, char **endptr) {
    float result = 0.0f;
    int sign = 1;
    float factor = 1.0f;

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
        result = result * 10.0f + (*str - '0');
        str++;
    }

    // Преобразование дробной части
    if (*str == '.') {
        str++;
        while ((*str >= '0' && *str <= '9')) {
            factor /= 10.0f;
            result += (*str - '0') * factor;
            str++;
        }
    }

    if (endptr) {
        *endptr = (char *)str;
    }

    return result * sign;
}
