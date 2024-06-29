#include <string.h>

char *strcat(char *s, const char *t){
    strcpy(s + strlen(s), t);
    return s;
}