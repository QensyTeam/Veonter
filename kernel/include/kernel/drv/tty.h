#ifndef _KERNEL_DRV_TTY_H
#define _KERNEL_DRV_TTY_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
size_t terminal_getcolumn(void);
size_t terminal_getRow();
void terminal_getstring(char* buffer);
void terminal_set_color(uint8_t color);
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
void terminal_update(void);
void terminal_clear_line(size_t y);
void terminal_clearscreen(void);
void terminal_scroll(void);
void check(void);
char terminal_getchar(void);

#endif
