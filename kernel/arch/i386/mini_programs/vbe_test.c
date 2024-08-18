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
    rgb_color_t expanding_rectangle_color = {0, 0, 160}; // Синий цвет
    rgb_color_t shrinking_rectangle_color = {255, 0, 0}; // Красный цвет

    // Расчет задержек
    uint32_t delay_per_column = 2000 / screen_width; // Задержка для закраски по столбцам, 2 секунды на весь цикл
    uint32_t delay_per_row = 2000 / screen_height;   // Задержка для закраски по строкам, 2 секунды на весь цикл

    // 1. Слева направо синий цвет
    for (int x = 0; x < screen_width; x++) {
        for (int y = 0; y < screen_height; y++) {
            putpixel(x, y, blue_color);
        }
        usleep(delay_per_column * 1000); // Преобразуем в микросекунды
    }

    // 2. Справа налево зеленый цвет
    for (int x = screen_width - 1; x >= 0; x--) {
        for (int y = 0; y < screen_height; y++) {
            putpixel(x, y, green_color);
        }
        usleep(delay_per_column * 1000); // Преобразуем в микросекунды
    }

    // 3. Сверху вниз красный цвет
    for (int y = 0; y < screen_height; y++) {
        for (int x = 0; x < screen_width; x++) {
            putpixel(x, y, red_color);
        }
        usleep(delay_per_row * 1000); // Преобразуем в микросекунды
    }

    // 4. Снизу вверх белый цвет
    for (int y = screen_height - 1; y >= 0; y--) {
        for (int x = 0; x < screen_width; x++) {
            putpixel(x, y, white_color);
        }
        usleep(delay_per_row * 1000); // Преобразуем в микросекунды
    }

    // Центр экрана
    int center_x = screen_width / 2;
    int center_y = screen_height / 2;

    // Общее количество шагов для прямоугольников
    int steps = 100;  // Количество шагов для плавного увеличения/сужения прямоугольника
    uint32_t delay_per_step = 2000 / steps; // Задержка на один шаг, 2 секунды на весь цикл

    // 5. Расширяющийся прямоугольник
    for (int step = 0; step < steps; step++) {
        // Вычисляем текущие размеры прямоугольника
        int current_width = (screen_width * step) / steps;
        int current_height = (screen_height * step) / steps;

        // Определяем координаты верхнего левого угла прямоугольника
        int top_left_x = center_x - current_width / 2;
        int top_left_y = center_y - current_height / 2;

        // Определяем координаты нижнего правого угла прямоугольника
        int bottom_right_x = center_x + current_width / 2;
        int bottom_right_y = center_y + current_height / 2;

        // Заливаем прямоугольник
        for (int y = top_left_y; y <= bottom_right_y; y++) {
            for (int x = top_left_x; x <= bottom_right_x; x++) {
                if (x >= 0 && x < screen_width && y >= 0 && y < screen_height) {
                    putpixel(x, y, expanding_rectangle_color);
                }
            }
        }

        // Задержка между шагами
        usleep(delay_per_step * 1000); // Преобразуем в микросекунды
    }

    // 6. Сужающийся прямоугольник
    for (int step = 0; step < steps; step++) {
        // Вычисляем текущие размеры прямоугольника
        int current_width = screen_width - (screen_width * step) / steps;
        int current_height = screen_height - (screen_height * step) / steps;

        // Определяем координаты верхнего левого угла прямоугольника
        int top_left_x = center_x - current_width / 2;
        int top_left_y = center_y - current_height / 2;

        // Определяем координаты нижнего правого угла прямоугольника
        int bottom_right_x = center_x + current_width / 2;
        int bottom_right_y = center_y + current_height / 2;

        // Заливаем только внешний контур текущего прямоугольника
        for (int x = top_left_x; x <= bottom_right_x; x++) {
            if (x >= 0 && x < screen_width) {
                if (top_left_y >= 0 && top_left_y < screen_height) {
                    putpixel(x, top_left_y, shrinking_rectangle_color);
                }
                if (bottom_right_y >= 0 && bottom_right_y < screen_height) {
                    putpixel(x, bottom_right_y, shrinking_rectangle_color);
                }
            }
        }
        for (int y = top_left_y; y <= bottom_right_y; y++) {
            if (y >= 0 && y < screen_height) {
                if (top_left_x >= 0 && top_left_x < screen_width) {
                    putpixel(top_left_x, y, shrinking_rectangle_color);
                }
                if (bottom_right_x >= 0 && bottom_right_x < screen_width) {
                    putpixel(bottom_right_x, y, shrinking_rectangle_color);
                }
            }
        }

        // Задержка между шагами
        usleep(delay_per_step * 1000); // Преобразуем в микросекунды
    }
}
