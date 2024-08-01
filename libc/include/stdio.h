#ifndef _STDIO_H
#define _STDIO_H 1

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdbool.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

int scanf(const char* format, ...);
int printf(const char* __restrict, ...);
int putchar(int);
int puts(const char*);

#ifdef __cplusplus
}
#endif

#endif
