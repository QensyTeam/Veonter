#include <string.h>

void substr(char* dest, const char* src, int start, int length) {
    size_t src_length = strlen(src);
    if (start < 0 || (size_t)start + length > src_length) {
        dest[0] = '\0';
        return;
    }
    strncpy(dest, src + start, length);
    dest[length] = '\0';
}
