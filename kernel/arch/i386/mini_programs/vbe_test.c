#include <kernel/mini_programs/mini_programs.h>
#include <kernel/kernel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern uint16_t screen_width;
extern uint16_t screen_height;

void vbe_test() {
    // Устанавливаем цвета
    rgb_color_t blue_color = {0, 0, 255};   // Синий цвет
    rgb_color_t green_color = {0, 255, 0};  // Зеленый цвет
    rgb_color_t red_color = {255, 0, 0};    // Красный цвет
    rgb_color_t white_color = {255, 255, 255}; // Белый цвет

    // Задержки для каждого направления
    uint32_t delay_per_column = 12000 / screen_width; // Для закраски по столбцам
    uint32_t delay_per_row = 8000 / screen_height;   // Для закраски по строкам

    // 1. Слева направо синий цвет
    for (int x = 0; x < screen_width; x++) {
        for (int y = 0; y < screen_height; y++) {
            putpixel(x, y, blue_color);
        }
        sleep(delay_per_column);
    }

    // 2. Справа налево зеленый цвет
    for (int x = screen_width - 1; x >= 0; x--) {
        for (int y = 0; y < screen_height; y++) {
            putpixel(x, y, green_color);
        }
        sleep(delay_per_column);
    }

    // 3. Сверху вниз красный цвет
    for (int y = 0; y < screen_height; y++) {
        for (int x = 0; x < screen_width; x++) {
            putpixel(x, y, red_color);
        }
        sleep(delay_per_row);
    }

    // 4. Снизу вверх белый цвет
    for (int y = screen_height - 1; y >= 0; y--) {
        for (int x = 0; x < screen_width; x++) {
            putpixel(x, y, white_color);
        }
        sleep(delay_per_row);
    }
}
