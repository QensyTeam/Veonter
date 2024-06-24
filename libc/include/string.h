#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>

#include <stddef.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


int memcmp(const void*, const void*, size_t);
void* memcpy(void* __restrict, const void* __restrict, size_t);
void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);
size_t strlen(const char*);

void substr(char* dest, const char* src, int start, int length);
char *strncpy(char*, const char*, size_t);
int strcpy(char*, const char*);
char *strcat(char*, const char*);
void* calloc(size_t, size_t);
int strcmp(const char*, const char*);
bool strcmpn(const char*, const char*);
long strtol(const char *str, char **endptr, int base);
long long strtoll(const char *str, char **endptr, int base);
double strtod(const char *str, char **endptr);
long double strtold(const char *str, char **endptr);
float strtof(const char *str, char **endptr);
const char* strchr(const char* str, int c);
void free(void* ptr);

void lftoa(long double value, char *str, int precision);
void ftoa(double value, char *str, int precision);
void lltoa(long long value, char *str, int base);
void ltoa(long value, char *str, int base);
void itoa(int value, char* str, int base);
void dtoa(double num, char* buffer, size_t precision);
void* malloc(size_t size);
void* realloc(void* ptr, size_t old_size, size_t new_size);
int strncmp(const char *s1, const char *s2, size_t n);
void* calloc(size_t num, size_t size);
void bzero(void *s, size_t n);

#ifdef __cplusplus
}
#endif

#endif
