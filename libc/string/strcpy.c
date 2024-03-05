#include <string.h>

/**
 * @brief Копирование строк
 *
 * @param char* dest - Указатель на строку, в которую будут скопированы данные
 * @param char* src - Указатель на строку источник копируемых данных
 *
 * @return int - Функция возвращает указатель на строку, в которую скопированы данные.
 */
int strcpy(char* dest, const char* src){
    int i = 0;

    while (src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';

    return i;
}