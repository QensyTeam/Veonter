#ifndef _KERNEL_DRV_CURSOR_H
#define _KERNEL_DRV_CURSOR_H
#include <stddef.h>
#include <stdint.h>

void terminal_enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void terminal_disable_cursor();
void terminal_update_cursor(int x, int y);
uint16_t terminal_get_cursor_position(void);

#endif