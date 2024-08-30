#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <kernel/kernel.h>

uint16_t screen_width;
uint16_t screen_height;
uint32_t* framebuffer;

static void vbe_write(uint16_t index, uint16_t data) {
    __asm__ volatile ("outw %0, %1" : : "a"(index), "Nd"(VBE_DISPI_IOPORT_INDEX));
    __asm__ volatile ("outw %0, %1" : : "a"(data), "Nd"(VBE_DISPI_IOPORT_DATA));
}

void set_video_mode(uint16_t width, uint16_t height, uint16_t bpp, uint32_t* fb_addr) {
    screen_width = width;
    screen_height = height;
    framebuffer = fb_addr;

    vbe_write(0x00, 0x4F02); 
    vbe_write(0x01, width);   
    vbe_write(0x02, height);  
    vbe_write(0x03, bpp);     
    vbe_write(0x04, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED); 
}

void putpixel(int x, int y, rgb_color_t color) {
    if (x >= 0 && x < screen_width && y >= 0 && y < screen_height) {
        uint32_t pixel_offset = y * screen_width + x;
        framebuffer[pixel_offset] = ((uint32_t)color.red << 16) | ((uint32_t)color.green << 8) | (uint32_t)color.blue;
    }
}

void fill_screen(rgb_color_t color) {
    for (int y = 0; y < screen_height; y++) {
        for (int x = 0; x < screen_width; x++) {
            putpixel(x, y, color);
        }
    }
}
