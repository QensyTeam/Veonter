#include <string.h>

#include <stdbool.h>

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s1 == *s2) {
        ++s1;
        ++s2;
    }
    return (*s1 - *s2);
}

bool strcmpn(const char *str1, const char *str2){
    return strcmp(str1, str2) == 0;
}