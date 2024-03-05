#include <string.h>

/**
 * @brief Копирование строк c ограничением длины
 *
 * @param char* dest - Указатель на строку, в которую будут скопированы данные
 * @param char* src - Указатель на строку источник копируемых данных
 * @param size_t n - Ограничение длинны копирования
 *
 * @return char* - Функция возвращает указатель на строку, в которую скопированы данные
 */
char *strncpy(char *dest, const char *src, size_t n){
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++)
        dest[i] = src[i];
    for (; i < n; i++)
        dest[i] = '\0';
    return dest;
}