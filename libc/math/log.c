#include <math.h>

#define M_E  2.71828182845904523536

double log(double x) {
    if (x <= 0) return -1;
    double result = 0;
    while (x >= M_E) {
        x /= M_E;
        result += 1;
    }
    double term = (x - 1) / (x + 1);
    double term_squared = term * term;
    double term_power = term;
    int n = 1;
    while (term_power > 1e-10) {
        result += term_power / n;
        term_power *= term_squared;
        n += 2;
    }
    return result;
}