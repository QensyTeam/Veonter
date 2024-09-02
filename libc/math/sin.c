#include <math.h>

double sin(double x) {
    double result = 0;
    double term = x;
    int n = 1;
    while (term > 1e-10 || term < -1e-10) {
        result += term;
        term *= -x * x / ((2 * n) * (2 * n + 1));
        n++;
    }
    return result;
}