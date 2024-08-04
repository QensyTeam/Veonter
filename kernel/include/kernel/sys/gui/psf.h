#ifndef PSF_H
#define PSF_H

#include <stdint.h>
#include "kernel/drv/vbe.h"

void fb_putchar(unsigned short int c, int cx, int cy, rgb_color_t fg, rgb_color_t bg);
void fb_puts(const char* str, int cx, int cy, rgb_color_t fg, rgb_color_t bg);
void psf_init();
void display_all_characters();

void psf_v1_init();
void fb_putchar_v1(unsigned short c, int cx, int cy, rgb_color_t fg, rgb_color_t bg);

#endif // PSF_H
