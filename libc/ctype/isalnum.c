#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>

int isalnum(int c) {
    return isalpha(c) || isdigit(c);
}
