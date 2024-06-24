#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>
#include <kernel/sys/gui/geometry.h>

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
