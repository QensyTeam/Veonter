#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <stdint.h>

void draw_square(int x, int y, int side, rgb_color_t color);
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, rgb_color_t color);
void draw_line(int x1, int y1, int x2, int y2, rgb_color_t color);
void draw_circle(int cx, int cy, int r, rgb_color_t color);
void draw_Rectangle(int x, int y, int width, int height, rgb_color_t color);

#endif // GEOMETRY_H
