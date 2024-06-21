#ifndef VBE_H
#define VBE_H

#include <stdint.h>

// RGB color structure
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb_color_t;

// Macro to create RGB color
#define RGB(r, g, b) ((rgb_color_t){(r), (g), (b)})

// VBE ports
#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA  0x01CF

// VBE control bits
#define VBE_DISPI_ENABLED      0x01
#define VBE_DISPI_LFB_ENABLED  0x40
#define VBE_DISPI_GETCAPS   0x02
#define VBE_DISPI_8BIT_DAC  0x20

#define VBE_DISPI_LFB_ENABLED       0x40
#define VBE_DISPI_NOCLEARMEM        0x80

#define VBE_DISPI_LFB_PHYSICAL_ADDRESS 0xB8000

// Function prototypes
void set_video_mode(uint16_t width, uint16_t height, uint16_t bpp, uint32_t* fb_addr);
void putpixel(int x, int y, rgb_color_t color);
void draw_square(int x, int y, int side, rgb_color_t color);
void fill_screen(rgb_color_t color);
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, rgb_color_t color);
void draw_line(int x1, int y1, int x2, int y2, rgb_color_t color);
void draw_circle(int cx, int cy, int r, rgb_color_t color);
void draw_Rectangle(int x, int y, int width, int height, rgb_color_t color);
void Window(int x, int y, int x1, int y1);
void fb_putchar(unsigned short int c, int cx, int cy, rgb_color_t fg, rgb_color_t bg);
void fb_puts(const char* str, int cx, int cy, rgb_color_t fg, rgb_color_t bg);
void psf_init();
void shell_putchar(unsigned short int c);
void initialize_screen();
void vbe_clear_screen(rgb_color_t color);
void shell_text_color(rgb_color_t color);
size_t shell_getcolumn(void);
void display_all_characters();

// Variables for screen properties and framebuffer
extern uint16_t screen_width;
extern uint16_t screen_height;
extern uint32_t* framebuffer;

#endif // VBE_H
