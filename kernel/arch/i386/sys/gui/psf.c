#include "kernel/sys/gui/psf.h"
#include "kernel/drv/vbe.h"
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

extern uint32_t _binary_font_psf_start;
extern char _binary_font_psf_end;

void* psf_glyphs;

void psf_v1_init() {
	PSF1_Header* hdr = (PSF1_Header*)&_binary_font_psf_start;
	if(hdr->magic != PSF1_FONT_MAGIC) {
		return;
	}

	psf_glyphs = (unsigned char*)(hdr + 1);
}

void fb_putchar(unsigned short c, int cx, int cy, rgb_color_t fg, rgb_color_t bg) {
	PSF1_Header* hdr = (PSF1_Header*)&_binary_font_psf_start;
	
	unsigned int size = hdr->characterSize;
    
    uint8_t lo = (uint8_t)(c >> 8);
    uint8_t hi = (uint8_t)(c & 0xff);
    
    if(hi == 0xd0 || hi == 0xd1) {
        uint16_t sym = lo & 0x3f;

        if(sym == 0) {
            c = 224;
        } else if(sym == 1) {
            c = hi == 0xd0 ? 240 : 225;
        } else if(sym >= 2 && sym <= 15) {
            c = 224 + sym;
        } else if(sym == 16) {
            c = 128;
        } else if(sym >= 18 && sym <= 63) {
            c = 128 + (sym - 16);
        } else if(sym == 17) {
            c = hi == 0xd1 ? 241 : 129;
        }
    }

	unsigned char* glyph = (unsigned char*)(psf_glyphs + (c * size));

	cx *= 8;
	cy *= size;

	for(unsigned int y = 0; y < size; y++) {
		for(int x = 0; x < 8; x++) {
			if(glyph[y] & (1 << (7 - x))) {
				putpixel(cx + x, cy + y, fg);
			} else {
				putpixel(cx + x, cy + y, bg);
			}	
		}
	}
}

void fb_puts(const char* str, int cx, int cy, rgb_color_t fg, rgb_color_t bg) {
    while (*str) {
        uint16_t ch = *str;

        if(ch == 0xd0 || ch == 0xd1) {
            ch <<= 8;
            ch |= *str++;
        }

        fb_putchar((unsigned short int)ch, cx, cy, fg, bg);
        cx++;
        str++;
    }
}
