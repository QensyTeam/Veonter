#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>
#include <kernel/kernel.h>

// Define default colors using RGB macro
#define DEFAULT_FG_COLOR RGB(255, 255, 255)  // Цвет текста по умолчанию
#define DEFAULT_BG_COLOR RGB(0, 0, 0)    // Фон по умолчанию

#pragma GCC diagnostic ignored "-Wunused-variable"
static rgb_color_t fg_color = DEFAULT_FG_COLOR;
static rgb_color_t bg_color = DEFAULT_BG_COLOR;
#pragma GCC diagnostic warning "-Wunused-variable"


void shell_putchar(unsigned short int c);
void shell_text_color(rgb_color_t color);
void logo();
void draw_cursor();
void remove_cursor();
void vbe_clear_screen(rgb_color_t color);
void initialize_screen();
void scroll_screen();
size_t vbe_getcolumn(void);

#endif // SHELL_H
