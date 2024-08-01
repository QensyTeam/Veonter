#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <kernel/kernel.h>

extern rgb_color_t fg_color;
extern rgb_color_t bg_color;

uint16_t *unicode;

extern char *fb;
extern char _binary_font_start[];

#define PIXEL uint32_t  

#define PSF1_FONT_MAGIC 0x0436

typedef struct {
    uint16_t magic; 
    uint8_t fontMode; 
    uint8_t characterSize; 
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

void display_all_characters() {
    PSF_font *font = (PSF_font*)&_binary_font_psf_start;

    int num_glyphs = font->numglyph;
    int max_cols = 16; 

    for (int y = 0; y < screen_height; y++) {
        for (int x = 0; x < screen_width; x++) {
            putpixel(x, y, bg_color); 
        }
    }

    for (int i = 0; i < num_glyphs; i++) {
        int cx = i % max_cols; 
        int cy = i / max_cols; 
        fb_putchar(i, cx, cy, fg_color, bg_color); 
    }
}

void fb_puts(const char* str, int cx, int cy, rgb_color_t fg, rgb_color_t bg) {
    while (*str) {
        fb_putchar((unsigned short int)*str, cx, cy, fg, bg);
        cx++;
        str++;
    }
}