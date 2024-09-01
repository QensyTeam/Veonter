#include <stddef.h>
#include <stdbool.h>

int iscntrl(int c) {
    return (c >= 0 && c < 32) || (c == 127);
}