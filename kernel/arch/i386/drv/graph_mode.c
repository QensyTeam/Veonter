#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <kernel/kernel.h>

// Define global variables here
uint16_t screen_width;
uint16_t screen_height;
uint32_t* framebuffer;

extern char *fb;
extern char _binary_font_start[];

#define PIXEL uint32_t   /* pixel pointer */

#define PSF1_FONT_MAGIC 0x0436

typedef struct {
    uint16_t magic; // Magic bytes for identification.
    uint8_t fontMode; // PSF font mode.
    uint8_t characterSize; // PSF character size.
} PSF1_Header;

#define PSF_FONT_MAGIC 0x864ab572

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t headersize;
    uint32_t flags;
    uint32_t numglyph;
    uint32_t bytesperglyph;
    uint32_t height;
    uint32_t width;
} PSF_font;

extern char _binary_font_psf_start;
extern char _binary_font_psf_end;

uint16_t *unicode;

#define CHAR_WIDTH 8  // Ширина символа в пикселях
#define CHAR_HEIGHT 16  // Высота символа в пикселях

uint16_t max_cols; // Максимальное количество символов в строке
uint16_t max_rows; // Максимальное количество строк на экране

rgb_color_t fg_color = RGB(255, 255, 255);  // Белый цвет текста по умолчанию
rgb_color_t bg_color = RGB(0, 0, 0);        // Черный фон по умолчанию


void initialize_screen() {
    max_cols = screen_width / CHAR_WIDTH;
    max_rows = screen_height / CHAR_HEIGHT;
}


void shell_text_color(rgb_color_t color) {
    fg_color = color;
}

void psf_init() {
    PSF_font *font = (PSF_font*)&_binary_font_psf_start;
    if (font->magic != PSF_FONT_MAGIC) {
        unicode = NULL;
        return;
    }

    // Определяем количество символов в шрифте (512)
    int num_glyphs = 512;

    unicode = calloc(USHRT_MAX, 2);
    unsigned char *glyph_ptr = (unsigned char *)&_binary_font_psf_start + font->headersize;

    // Пробегаем по каждому символу в шрифте
    for (int i = 0; i < num_glyphs; i++) {
        unicode[i] = i;
        glyph_ptr += font->bytesperglyph;
    }
}

int scanline = 2048;

void fb_putchar(unsigned short int c, int cx, int cy, rgb_color_t fg, rgb_color_t bg) {
    PSF_font *font = (PSF_font*)&_binary_font_psf_start;
    int bytesperline = (font->width + 7) / 8; // Ширина символа в байтах
    if (unicode != NULL) {
        c = unicode[c];
    }
    unsigned char *glyph = (unsigned char*)&_binary_font_psf_start +
                           font->headersize +
                           (c > 0 && c < font->numglyph ? c : 0) * font->bytesperglyph;
    
    // Рассчитываем смещение в пикселях, без лишнего пространства между символами
    int offs = (cy * font->height * screen_width) + (cx * font->width);
    unsigned int x, y, line, mask;
    for (y = 0; y < font->height; y++) {
        line = offs;
        mask = 1 << (font->width - 1);
        for (x = 0; x < font->width; x++) {
            // Определяем, отрисовывать ли пиксель из символа или оставить фоновый цвет
            if (*((unsigned int*)glyph) & mask) {
                putpixel(line % screen_width, line / screen_width, fg);
            } else {
                putpixel(line % screen_width, line / screen_width, bg);
            }
            mask >>= 1;
            line++;
        }
        glyph += bytesperline;
        offs += screen_width;
    }
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

static int cursor_x = 0;  // Текущая x-координата курсора
static int cursor_y = 0;  // Текущая y-координата курсора

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

void display_all_characters() {
    PSF_font *font = (PSF_font*)&_binary_font_psf_start;

    int num_glyphs = font->numglyph;
    int max_cols = 16; // Example: display 16 characters per row
    int max_rows = (num_glyphs + max_cols - 1) / max_cols; // Calculate number of rows needed

    // Clear the screen
    for (int y = 0; y < screen_height; y++) {
        for (int x = 0; x < screen_width; x++) {
            putpixel(x, y, (rgb_color_t){0, 0, 0}); // Set background color
        }
    }

    // Render each glyph in a grid
    for (int i = 0; i < num_glyphs; i++) {
        int cx = i % max_cols; // Column index
        int cy = i / max_cols; // Row index
        // Render the character glyph
        fb_putchar(i, cx, cy, (rgb_color_t){255, 255, 255}, (rgb_color_t){0, 0, 0}); // White text on black background
    }
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



// Function to write to a VBE register
static void vbe_write(uint16_t index, uint16_t data) {
    asm volatile ("outw %0, %1" : : "a"(index), "Nd"(VBE_DISPI_IOPORT_INDEX));
    asm volatile ("outw %0, %1" : : "a"(data), "Nd"(VBE_DISPI_IOPORT_DATA));
}

// Set VBE video mode
void set_video_mode(uint16_t width, uint16_t height, uint16_t bpp, uint32_t* fb_addr) {
    screen_width = width;
    screen_height = height;
    framebuffer = fb_addr;

    vbe_write(0x00, 0x4F02); // Set VBE mode
    vbe_write(0x01, width);   // Set width
    vbe_write(0x02, height);  // Set height
    vbe_write(0x03, bpp);     // Set bits per pixel
    vbe_write(0x04, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED); // Enable LFB mode
}

// Set a pixel at (x, y) with the specified color
void putpixel(int x, int y, rgb_color_t color) {
    if (x >= 0 && x < screen_width && y >= 0 && y < screen_height) {
        uint32_t pixel_offset = y * screen_width + x;
        framebuffer[pixel_offset] = ((uint32_t)color.red << 16) | ((uint32_t)color.green << 8) | (uint32_t)color.blue;
    }
}

// Fill the entire screen with a specific color
void fill_screen(rgb_color_t color) {
    for (int y = 0; y < screen_height; y++) {
        for (int x = 0; x < screen_width; x++) {
            putpixel(x, y, color);
        }
    }
}

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

void Window(int x, int y, int x1, int y1)
{
    draw_Rectangle(x, y, x1, y1, RGB(195, 195, 195));
    draw_Rectangle(x + 2, y + 2, x1 - 4, 25, RGB(123, 62, 173));
    draw_Rectangle(x + 2, y + 29, x1 - 4, y1 - 31, RGB(127, 127, 127));
    draw_circle(x + x1 - 15, y + 14, 9, RGB(255, 0, 0));
    //draw_square(x, y, 200, RGB(195, 195, 195));
    //draw_square(x + 2, y + 2, 100, RGB(123, 62, 173));
}





void fb_puts(const char* str, int cx, int cy, rgb_color_t fg, rgb_color_t bg) {
    while (*str) {
        fb_putchar((unsigned short int)*str, cx, cy, fg, bg);
        cx++;
        str++;
    }
}

size_t shell_getcolumn(void) {
 	return max_cols;
}
