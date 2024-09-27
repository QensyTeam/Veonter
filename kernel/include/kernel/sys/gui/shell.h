#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>
#include <kernel/drv/vbe.h>
#include <stddef.h>

extern volatile int cursor_x;  // Текущая x-координата курсора
extern volatile int cursor_y;  // Текущая y-координата курсора

void shell_putchar(unsigned short int c);
void shell_text_color(rgb_color_t color);
void logo();
void enable_cursor();
void disable_cursor();
void vbe_clear_screen(rgb_color_t color);
void initialize_screen();
void scroll_screen();
size_t vbe_getcolumn(void);
void colors_program();

#endif // SHELL_H
