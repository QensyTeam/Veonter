#include <string.h>

long strtol(const char *str, char **endptr, int base) {
    long result = 0;
    int sign = 1;

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

    // Преобразование строки в число
    while ((*str >= '0' && *str <= '9')) {
        result = result * base + (*str - '0');
        str++;
    }

    if (endptr) {
        *endptr = (char *)str;
    }

    return result * sign;
}