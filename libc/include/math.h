#include <stddef.h>
#include <stdint.h>

#ifndef MATH_CUSTOM_H
#define MATH_CUSTOM_H

#ifdef __cplusplus
extern "C" {
#endif

#define M_PI 3.14159265358979323846
#define M_E  2.71828182845904523536

double fabs(double x);
double floor(double x);
double ceil(double x);
double sqrt(double x);
double pow(double base, double exp);
double exp(double x);
double log(double x);
double log10(double x);
double sin(double x);
double cos(double x);
double tan(double x);

#ifdef __cplusplus
}
#endif

#endif // MATH_CUSTOM_H
