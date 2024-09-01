#include <stddef.h>
#include <stdbool.h>

int isalnum(int c) {
    return isalpha(c) || isdigit(c);
}