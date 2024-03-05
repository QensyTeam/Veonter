#include <string.h>

#include <stdbool.h>

/**
 * @brief Сравнение строк
 *
 * @param char* s1 - Строка 1
 * @param char* s2 - Строка 2
 *
 * @return int - Возращает 0 если строки идентичны или разницу между ними
 */
int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s1 == *s2) {
        ++s1;
        ++s2;
    }
    return (*s1 - *s2);
}

/**
 * @brief Сравнение строк
 *
 * @param char* s1 - Строка 1
 * @param char* s2 - Строка 2
 *
 * @return bool - Возращает true если строки идентичны
 */
bool strcmpn(const char *str1, const char *str2){
    return strcmp(str1, str2) == 0;
}