#include <stdint.h>
#include <string.h>

char *strncat(char *dest, const char *src, size_t n) {
    if (dest == NULL) {
        return NULL;
    }

    char *end = dest + strlen(dest);

    while (*src != '\0' && n > 0) {
        *end++ = *src++;
        n--;
    }

    if (n > 0) {
        *end = '\0';
    }

    return dest;
}

