#include "kernel/drv/vbe.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <kernel/kernel.h>
#include <kernel/time.h>
#include <kernel/drv/rtc.h>

#define NUM_COLORS 16

typedef struct {
    uint8_t r, g, b;
} Color;

static Color colors[NUM_COLORS] = {
    {0, 0, 0},       // Black
    {128, 0, 0},     // Maroon
    {0, 128, 0},     // Green
    {128, 128, 0},   // Olive
    {0, 0, 128},     // Navy
    {128, 0, 128},   // Purple
    {0, 128, 128},   // Teal
    {192, 192, 192}, // Silver
    {128, 128, 128}, // Gray
    {255, 0, 0},     // Red
    {0, 255, 0},     // Lime
    {255, 255, 0},   // Yellow
    {0, 0, 255},     // Blue
    {255, 0, 255},   // Fuchsia
    {0, 255, 255},   // Aqua
    {255, 255, 255}  // White
};

rgb_color_t fg_color = RGB(255, 255, 255);
rgb_color_t bg_color = RGB(0, 0, 0);
rgb_color_t main_color;

volatile int cursor_x = 0;  // Текущая x-координата курсора
volatile int cursor_y = 0;  // Текущая y-координата курсора
volatile bool cursor_visible = true; // Видимость курсора

uint16_t max_cols; // Максимальное количество символов в строке
uint16_t max_rows; // Максимальное количество строк на экране

char* Version = "\t\t\t\t\t\t\t\t\t\t\t\t\t\tVersion: 0.0.1 Wolf(Pre-Alpha)\n\n";

char* title = 
"\n\t\t\t\t\t\t##  ## #####  ####  ##  ## ###### ##### ##### \n\
\t\t\t\t\t\t##  ## ##    ##  ## ### ##   ##   ##    ##  ##\n\
\t\t\t\t\t\t##  ## ####  ##  ## ## ###   ##   ####  ##### \n\
\t\t\t\t\t\t ####  ##    ##  ## ##  ##   ##   ##    ##  ##\n\
\t\t\t\t\t\t  ##   #####  ####  ##  ##   ##   ##### ##  ##\n\n";

void logo() {
    main_color = fg_color;
    shell_text_color(RGB(140, 0, 255));
    printf("%s", title);
    shell_text_color(RGB(255, 255, 255));
    printf("\t\t\t\t\t\t\t\t    Made by the Qensy Team\n\n");
    shell_text_color(RGB(0, 252, 29));
	printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tWelcome!\n");
	shell_text_color(RGB(112, 112, 112));
	printf("%s", Version);
	shell_text_color(main_color);
    printf("\tType <help> to see all possible commands for using this console.\n\n\n");
    time_t t = get_time();
    printf("Current time: %02d:%02d:%02d\n", t.hour, t.minute, t.second);
}

void shell_text_color(rgb_color_t color) {
    fg_color = color;
}

void set_text_color(uint8_t index) {
    if (index < NUM_COLORS) {
        fg_color = RGB(colors[index].r, colors[index].g, colors[index].b);
    }
}

void set_background_color(uint8_t index) {
    if (index < NUM_COLORS) {
        bg_color = RGB(colors[index].r, colors[index].g, colors[index].b);
    }
}

void colors_program() {
    uint8_t index;

    printf("Available colors:\n");
    printf("0: Black\n1: Maroon\n2: Green\n3: Olive\n4: Navy\n5: Purple\n6: Teal\n7: Silver\n");
    printf("8: Gray\n9: Red\n10: Lime\n11: Yellow\n12: Blue\n13: Fuchsia\n14: Aqua\n15: White\n");

    printf("Enter new text color index (0-15): ");
    if (scanf("%d", &index) == 1) {
        set_text_color(index);
        printf("Text color set to %d\n", index);
    } else {
        printf("Invalid input format for text color.\n");
    }

    printf("Enter new background color index (0-15): ");
    if (scanf("%d", &index) == 1) {
        set_background_color(index);
        printf("Background color set to %d\n", index);
        vbe_clear_screen(bg_color); // Очищаем экран с новым цветом фона
    } else {
        printf("Invalid input format for background color.\n");
    }
}

void enable_cursor() {
    // Рисуем курсор как нижнее подчеркивание высотой 2 пикселя
    draw_Rectangle(cursor_x * CHAR_WIDTH, (cursor_y + 1) * CHAR_HEIGHT - 1, CHAR_WIDTH, 2, fg_color);
}

void disable_cursor() {
    // Очищаем то место, где был курсор
    draw_Rectangle(cursor_x * CHAR_WIDTH, (cursor_y + 1) * CHAR_HEIGHT - 1, CHAR_WIDTH, 2, bg_color);
}

void update_cursor_position() {
    if (cursor_visible) {
        disable_cursor();
        enable_cursor();
    }
}

void shell_putchar(unsigned short int c) {
    disable_cursor(); // Отключаем курсор перед выводом символа
    switch(c) {
        case '\n': // Новая строка
            cursor_x = 0;
            cursor_y++;
            if (cursor_y >= max_rows) {
                scroll_screen();
                cursor_y = max_rows - 1;
            }
            break;
        case '\b': // Backspace
            if (cursor_x > 0) {
                cursor_x--;
                fb_putchar(' ', cursor_x, cursor_y, fg_color, bg_color);
            } else if (cursor_y > 0) {
                cursor_y--;
                cursor_x = max_cols - 1;
                fb_putchar(' ', cursor_x, cursor_y, fg_color, bg_color);
            }
            break;
        case '\r': // Возврат каретки
            cursor_x = 0;
            break;
        case '\t': // Табуляция
            cursor_x = (cursor_x + 4) & ~(4 - 1); // Перенос на 4 символа вперед
            if (cursor_x >= max_cols) {
                cursor_x = 0;
                cursor_y++;
            }
            break;
        default:
            fb_putchar(c, cursor_x, cursor_y, fg_color, bg_color);
            cursor_x++;
            if (cursor_x >= max_cols) {
                cursor_x = 0;
                cursor_y++;
                if (cursor_y >= max_rows) {
                    scroll_screen();
                    cursor_y = max_rows - 1;
                }
            }
            break;
    }
    update_cursor_position(); // Обновляем позицию курсора после вывода символа
}

void vbe_clear_screen(rgb_color_t color) {
    for (int y = 0; y < screen_height; y++) {
        for (int x = 0; x < screen_width; x++) {
            putpixel(x, y, color);  // Заполняем каждый пиксель экрана заданным цветом
        }
    }
    cursor_x = 0;
    cursor_y = 0;
}

void initialize_screen() {
    max_cols = screen_width / CHAR_WIDTH;
    max_rows = screen_height / CHAR_HEIGHT;
    vbe_clear_screen(bg_color);
    printf("VBE initialization completed successfully!\n");
}

void scroll_screen() {
    memmove(framebuffer, framebuffer + (CHAR_HEIGHT * screen_width), 
            (screen_height - CHAR_HEIGHT) * screen_width * sizeof(uint32_t));
    // Очищаем последнюю строку
    for (int x = 0; x < max_cols; x++) {
        fb_putchar(' ', x, max_rows - 1, fg_color, bg_color);
    }
}

size_t vbe_getcolumn(void) {
 	return max_cols;
}
