#include <math.h>

double sqrt(double x) {
    if (x < 0) return -1; // Ошибка: корень из отрицательного числа
    double result = x;
    double epsilon = 1e-10;
    while ((result - x / result) > epsilon) {
        result = (result + x / result) / 2;
    }
    return result;
}