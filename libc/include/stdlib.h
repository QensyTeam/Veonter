#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort(void);
char* reverseString(char *string);
int intToStr(int x, char str[], int d);
void reverse(char* str, int len);
void* calloc(size_t, size_t);
void free(void* ptr);
void lftoa(long double value, char *str, int precision);
void ftoa(double value, char *str, int precision);
void lltoa(long long value, char *str, int base);
void lutoa(unsigned long value, char *str, int base);
void llutoa(unsigned long long value, char *str, int base);
void ltoa(long value, char *str, int base);
void itoa(int value, char *buffer, int base);
void dtoa(double num, char* buffer, size_t precision);
void gtoa(double value, char *buffer, int precision);
void etoa(double value, char* buffer, int precision);
void* malloc(size_t size);
void* realloc(void* ptr, size_t new_size);
void utoa(unsigned int value, char *buffer, int base);
void* calloc(size_t num, size_t size);
unsigned long long strtoull(const char* str, char** endptr, int base);
unsigned long strtoul(const char* str, char** endptr, int base);
void srand(unsigned int new_seed);
int rand(void);
#ifdef __cplusplus
}
#endif

#endif

