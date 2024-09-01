#include <math.h>

double exp(double x) {
    double result = 1;
    double term = 1;
    int n = 1;
    while (term > 1e-10) {
        term *= x / n++;
        result += term;
    }
    return result;
}
