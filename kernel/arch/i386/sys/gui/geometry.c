#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <kernel/kernel.h>

int abs(int x) {
    return (x < 0) ? -x : x;
}

// Draw a circle centered at (cx, cy) with radius r and color
void draw_circle(int cx, int cy, int r, rgb_color_t color) {
    int x = 0, y = r;
    int d = 3 - 2 * r;
    while (x <= y) {
        // Draw scanlines from top to bottom of the circle
        for (int i = cx - x; i <= cx + x; i++) {
            putpixel(i, cy + y, color); // Top half of the circle
            putpixel(i, cy - y, color); // Bottom half of the circle
        }
        for (int i = cx - y; i <= cx + y; i++) {
            putpixel(i, cy + x, color); // Top half of the circle
            putpixel(i, cy - x, color); // Bottom half of the circle
        }

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}


// Draw a square with top-left corner at (x, y), side length side, and color
void draw_square(int x, int y, int side, rgb_color_t color) {
    for (int i = x; i < x + side; i++) {
        for (int j = y; j < y + side; j++) {
            putpixel(i, j, color);
        }
    }
}

void draw_Rectangle(int x, int y, int width, int height, rgb_color_t color) {
    for (int i = x; i < x + width; i++) {
        for (int j = y; j < y + height; j++) {
            putpixel(i, j, color);
        }
    }
}

// Draw a line from (x1, y1) to (x2, y2) with color
void draw_line(int x1, int y1, int x2, int y2, rgb_color_t color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx, sy;
    if (x1 < x2) sx = 1; else sx = -1;
    if (y1 < y2) sy = 1; else sy = -1;
    int err = dx - dy;

    while (true) {
        putpixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }
    }
}

// Draw a triangle with vertices (x1, y1), (x2, y2), (x3, y3) and color
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, rgb_color_t color) {
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x3, y3, color);
    draw_line(x3, y3, x1, y1, color);
}