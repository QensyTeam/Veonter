#include <math.h>

#define M_PI 3.14159265358979323846

double cos(double x) {
    return sin(M_PI / 2 - x);
}
