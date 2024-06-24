#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <kernel/kernel.h>

rgb_color_t fg_color = RGB(255, 255, 255);  // Белый цвет текста по умолчанию
rgb_color_t bg_color = RGB(0, 0, 0);        // Черный фон по умолчанию

static int cursor_x = 0;  // Текущая x-координата курсора
static int cursor_y = 0;  // Текущая y-координата курсора

uint16_t max_cols; // Максимальное количество символов в строке
uint16_t max_rows; // Максимальное количество строк на экране


char* title = 
"\n\t\t\t\t\t\t##  ## #####  ####  ##  ## ###### ##### ##### \n\
\t\t\t\t\t\t##  ## ##    ##  ## ### ##   ##   ##    ##  ##\n\
\t\t\t\t\t\t##  ## ####  ##  ## ## ###   ##   ####  ##### \n\
\t\t\t\t\t\t ####  ##    ##  ## ##  ##   ##   ##    ##  ##\n\
\t\t\t\t\t\t  ##   #####  ####  ##  ##   ##   ##### ##  ##\n\n";

char* Version = "\t\t\t\t\t\t\t\t\t\t\t\t\t\tVersion: 0.0.1 Wolf(Pre-Alpha)\n\n";

void logo() {
    shell_text_color(RGB(140, 0, 255));
    printf(title);
    shell_text_color(RGB(0, 252, 29));
	printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tWelcome!\n");
	shell_text_color(RGB(112, 112, 112));
	printf(Version);
	shell_text_color(RGB(255, 255, 255));
}

void shell_text_color(rgb_color_t color) {
    fg_color = color;
}

void shell_putchar(unsigned short int c) {
    remove_cursor();

    // Обработка специальных символов
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= max_rows) {
            scroll_screen();
            cursor_y = max_rows - 1;
        }
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            fb_putchar(' ', cursor_x, cursor_y, RGB(0, 0, 0), RGB(0, 0, 0)); // Очищаем символ
        } else if (cursor_y > 0) {
            cursor_y--;
            cursor_x = max_cols - 1;
            fb_putchar(' ', cursor_x, cursor_y, RGB(0, 0, 0), RGB(0, 0, 0));
        }
    } else if (c == '\t') {
        // Обработка символа табуляции
        for (int i = 0; i < 4; i++) {
            shell_putchar(' ');
        }
    } else if (c == '\v') {
        // Обработка вертикальной табуляции
        cursor_y++;
        if (cursor_y >= max_rows) {
            scroll_screen();
            cursor_y = max_rows - 1;
        }
    } else if (c == '\f') {
        // Обработка очистки экрана
        vbe_clear_screen(RGB(0,0,0));
        cursor_x = 0;
        cursor_y = 0;
    } else if (c == '\r') {
        // Обработка возврата каретки
        cursor_x = 0;
    } else {
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
    }

    draw_cursor();
}

void remove_cursor() {
    draw_Rectangle(cursor_x * CHAR_WIDTH, cursor_y * CHAR_HEIGHT, CHAR_WIDTH, CHAR_HEIGHT, RGB(0, 0, 0));
}

void draw_cursor() {
    draw_Rectangle(cursor_x * CHAR_WIDTH, cursor_y * CHAR_HEIGHT, CHAR_WIDTH, CHAR_HEIGHT, RGB(255, 255, 255));
}

void vbe_clear_screen(rgb_color_t color) {
    cursor_x = 0;
    cursor_y = 0;
    for (int y = 0; y < screen_height; y++) {
        for (int x = 0; x < screen_width; x++) {
            putpixel(x, y, color);  // Заполняем каждый пиксель экрана заданным цветом
        }
    }
}

void initialize_screen() {
    max_cols = screen_width / CHAR_WIDTH;
    max_rows = screen_height / CHAR_HEIGHT;
}


void scroll_screen() {
    // Копируем каждую строку вверх на одну строку
    for (int y = 1; y < max_rows; y++) {
        for (int x = 0; x < max_cols; x++) {
            uint32_t* src = framebuffer + (y * CHAR_HEIGHT * screen_width) + (x * CHAR_WIDTH);
            uint32_t* dst = framebuffer + ((y - 1) * CHAR_HEIGHT * screen_width) + (x * CHAR_WIDTH);
            for (int i = 0; i < CHAR_HEIGHT; i++) {
                memcpy(dst, src, CHAR_WIDTH * sizeof(uint32_t));
                dst += screen_width;
                src += screen_width;
            }
        }
    }
    // Очищаем последнюю строку
    for (int x = 0; x < max_cols; x++) {
        fb_putchar(' ', x, max_rows - 1, RGB(0, 0, 0), RGB(0, 0, 0));
    }
}

size_t vbe_getcolumn(void) {
 	return max_cols;
}