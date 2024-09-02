#include <math.h>

double ceil(double x) {
    return (x > 0) ? (double)((int)x + 1) : (double)((int)x);
}