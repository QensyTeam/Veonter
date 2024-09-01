#include <stdint.h>
#include <stdio.h>

static uint32_t seed = 1; // Начальное значение для генератора случайных чисел

// Функция для установки начального значения генератора случайных чисел
void srand(unsigned int new_seed) {
    seed = new_seed;
}