#include <string.h>

/**
 * @brief Вырезает и возвращает подстроку из строки
 *
 * @param char* dest - Указатель куда будет записана строка
 * @param char* source - Указатель на исходную строку
 * @param int source - Откуда копируем
 * @param size_t source - Количество копируемых строк
 */
void substr(char *dest, const char *source, int from, int length){
    strncpy(dest, source + from, length);
    dest[length] = 0;
} 
