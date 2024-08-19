#include <string.h>

double strtod(const char *str, char **endptr) {
    double result = 0.0;
    double sign = 1.0;
    double factor = 1.0;
    int exp_sign = 1;
    int exponent = 0;
    int digit;

    // Пропуск пробельных символов
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r' || *str == '\f' || *str == '\v') {
        str++;
    }

    // Проверка знака числа
    if (*str == '-') {
        sign = -1.0;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // Преобразование целой части
    while (*str >= '0' && *str <= '9') {
        digit = *str - '0';
        result = result * 10.0 + digit;
        str++;
    }

    // Преобразование дробной части
    if (*str == '.') {
        str++;
        factor = 0.1;
        while (*str >= '0' && *str <= '9') {
            digit = *str - '0';
            result += digit * factor;
            factor *= 0.1;
            str++;
        }
    }

    // Преобразование экспоненциальной части
    if (*str == 'e' || *str == 'E') {
        str++;
        if (*str == '-') {
            exp_sign = -1;
            str++;
        } else if (*str == '+') {
            str++;
        }

        while (*str >= '0' && *str <= '9') {
            digit = *str - '0';
            exponent = exponent * 10 + digit;
            str++;
        }

        // Умножаем результат на 10 в степени exponent, с учётом знака
        double exp_factor = 1.0;
        for (int i = 0; i < exponent; i++) {
            exp_factor *= 10.0;
        }
        if (exp_sign == -1) {
            result /= exp_factor;
        } else {
            result *= exp_factor;
        }
    }

    // Применение знака
    result *= sign;

    // Установка указателя на остаток строки
    if (endptr) {
        *endptr = (char *)str;
    }

    return result;
}
