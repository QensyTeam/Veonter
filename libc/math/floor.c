#include <math.h>

double floor(double x) {
    return (x >= 0) ? (double)((int)x) : (double)((int)x - 1);
}