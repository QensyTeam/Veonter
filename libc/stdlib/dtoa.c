#include <string.h>
#include <stdlib.h>

void dtoa(double num, char* buffer, size_t precision) {
    long long integerPart = (long long)num;
    double fractionalPart = num - integerPart;

    // Преобразуем целую часть числа в строку
    itoa(integerPart, buffer, 10);

    // Добавляем десятичную точку
    strcat(buffer, ".");

    // Преобразуем дробную часть числа в строку
    for (size_t i = 0; i < precision; i++) {
        fractionalPart *= 10;
        long long digit = (long long)fractionalPart;
        char digitChar = '0' + digit;
        strcat(buffer, &digitChar);
        fractionalPart -= digit;
    }
}