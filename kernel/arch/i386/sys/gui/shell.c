#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <kernel/kernel.h>

rgb_color_t fg_color = RGB(255, 255, 255);
rgb_color_t bg_color = RGB(0, 0, 0);
rgb_color_t main_color;

char* title = 
"\n\t\t\t\t\t\t##  ## #####  ####  ##  ## ###### ##### ##### \n\
\t\t\t\t\t\t##  ## ##    ##  ## ### ##   ##   ##    ##  ##\n\
\t\t\t\t\t\t##  ## ####  ##  ## ## ###   ##   ####  ##### \n\
\t\t\t\t\t\t ####  ##    ##  ## ##  ##   ##   ##    ##  ##\n\
\t\t\t\t\t\t  ##   #####  ####  ##  ##   ##   ##### ##  ##\n\n";

char* Version = "\t\t\t\t\t\t\t\t\t\t\t\t\t\tVersion: 0.0.1 Wolf(Pre-Alpha)\n\n";

void logo() {
    main_color = fg_color;
    shell_text_color(RGB(140, 0, 255));
    printf(title);
    shell_text_color(RGB(0, 252, 29));
	printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tWelcome!\n");
	shell_text_color(RGB(112, 112, 112));
	printf(Version);
	shell_text_color(main_color);
}

void shell_text_color(rgb_color_t color) {
    fg_color = color;
}

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

// Функция для изменения цвета текста
void set_text_color(uint8_t index) {
    if (index < NUM_COLORS) {
        fg_color = RGB(colors[index].r, colors[index].g, colors[index].b);
    }
}

// Функция для изменения цвета фона
void set_background_color(uint8_t index) {
    if (index < NUM_COLORS) {
        bg_color = RGB(colors[index].r, colors[index].g, colors[index].b);
    }
}

// Основная функция для работы с командой colors
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

int cursor_x = -1;  // Текущая x-координата курсора
int cursor_y = -1;  // Текущая y-координата курсора
bool cursor_visible = true; // Видимость курсора

uint16_t max_cols; // Максимальное количество символов в строке
uint16_t max_rows; // Максимальное количество строк на экране

void enable_cursor() {
    draw_Rectangle(cursor_x * CHAR_WIDTH, cursor_y * CHAR_HEIGHT, CHAR_WIDTH, CHAR_HEIGHT, fg_color);
}

void disable_cursor() {
    if (cursor_x != -1 && cursor_y != -1) {
        draw_Rectangle(cursor_x * CHAR_WIDTH, cursor_y * CHAR_HEIGHT, CHAR_WIDTH, CHAR_HEIGHT, bg_color);
    }
}

void shell_putchar(unsigned short int c) {
    // Обработка специальных символов
    if (c == '\n') {
        disable_cursor();
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= max_rows) {
            scroll_screen();
            cursor_y = max_rows - 1;
        }
    } else if (c == '\b') {
        if (cursor_x > 0) {
            disable_cursor();
            cursor_x--;
            fb_putchar(' ', cursor_x, cursor_y, bg_color, bg_color); // Очищаем символ
        } else if (cursor_y > 0) {
            cursor_y--;
            cursor_x = max_cols - 1;
            fb_putchar(' ', cursor_x, cursor_y, bg_color, bg_color);
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
        vbe_clear_screen(bg_color);
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
    check();
	printf("VBE initialization completed successfully!\n");
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
        fb_putchar(' ', x, max_rows - 1, bg_color, bg_color);
    }
}

size_t vbe_getcolumn(void) {
 	return max_cols;
}
