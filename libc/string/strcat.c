#include <string.h>

/**
 * @brief Объединение строк
 *
 * @param char* s - Указатель на массив в который будет добавлена строка
 * @param char* t - Указатель на массив из которого будет скопирована строка
 *
 * @return char* - Функция возвращает указатель на массив, в который добавлена строка
 */
char *strcat(char *s, const char *t){
    strcpy(s + strlen(s), t);
    return s;
}