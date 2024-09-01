#include <stddef.h>
#include <stdbool.h>

int tolower(int c) {
    return isupper(c) ? c + ('a' - 'A') : c;
}