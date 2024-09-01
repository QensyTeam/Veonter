#include <math.h>

double pow(double base, double exp) {
    double result = 1;
    while (exp > 0) {
        if ((int)exp % 2 == 1) {
            result *= base;
        }
        base *= base;
        exp /= 2;
    }
    return result;
}