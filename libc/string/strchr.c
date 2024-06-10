#include <stddef.h> // Для определения NULL

const char* strchr(const char* str, int c) {
    while (*str != '\0') {
        if (*str == c) {
            return str;
        }
        str++;
    }
    // Если символ не найден, возвращаем NULL
    return NULL;
}
