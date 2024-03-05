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

void substr(char*, const char*, int, int);
char *strncpy(char*, const char*, size_t);
int strcpy(char*, const char*);
char *strcat(char*, const char*);
void* calloc(size_t, size_t);
int strcmp(const char*, const char*);
bool strcmpn(const char*, const char*);
#ifdef __cplusplus
}
#endif

#endif
