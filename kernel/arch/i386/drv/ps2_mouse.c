#include <stdint.h>
#include <stdbool.h>

#include <kernel/sys/ports.h>
#include <kernel/sys/isr.h>
#include <kernel/drv/ps2_mouse.h>
#include <kernel/drv/ps2.h>

uint32_t mouse_x  = 0;           
uint32_t mouse_y  = 0;           

uint32_t mouse_max_x = 0;
uint32_t mouse_max_y = 0;

uint32_t mouse_ox  = 0;           
uint32_t mouse_oy  = 0;           

uint8_t mouse_buttons = 0;           

int mouse_wheel = 0;            

void mouse_parse_packet(const char *buf, uint8_t has_wheel, uint8_t has_5_buttons) {
    mouse_flags_byte *mfb = (mouse_flags_byte*) (buf);
    if (mfb->x_overflow || mfb->y_overflow || !mfb->always1) {
        return;
    }

    int offx = (int16_t) (0xff00 * mfb->x_sign) | buf[1];
    int offy = -((int16_t) (0xff00 * mfb->y_sign) | buf[2]);
   
    if(offx > 0 || mouse_x >= (uint32_t)-offx) {
        mouse_x += offx;
    }

    if(offy > 0 || mouse_y >= (uint32_t)-offy) {
        mouse_y += offy;
    }
   
    mouse_buttons = 0;

    if(mfb->left_button) {
        mouse_buttons |= MOUSE_BUTTON_LEFT;
    }

    if(mfb->right_button) {
        mouse_buttons |= MOUSE_BUTTON_RIGHT;
    }

    if(mfb->middle_button) {
        mouse_buttons |= MOUSE_BUTTON_MIDDLE;
    }

    if (has_wheel) {
        mouse_wheel += (char) ((!!(buf[3] & 0x8)) * 0xf8 | (buf[3] & 0x7));
        if (has_5_buttons) {
            if(!!(buf[3] & 0x20)) {
                mouse_buttons &= ~MOUSE_BUTTON_4;
            }
        }
    }
}

void mouse_handler(__attribute__((unused)) struct registers r) {
    uint8_t status = inb(0x64);
    if ((status & 1) == 0 || (status >> 5 & 1) == 0) {
        return;
    }

    static int recbyte = 0;
    static char mousebuf[5];

    mousebuf[recbyte++] = inb(0x60);
    if (recbyte == 3 /* + has_wheel */) {
        recbyte = 0;

        mouse_parse_packet(mousebuf, 1, 0);

        // Bounds
        if (mouse_x > mouse_max_x) {
            mouse_x = mouse_max_x;
        }
        if (mouse_y > mouse_max_y) {
            mouse_y = mouse_max_y;
        }
    }
}

void mouse_wait(uint8_t a_type) {
    uint32_t _time_out = 100;
    if (a_type == 0) {
        while (_time_out--) { //Data
            if ((inb(0x64) & 1) == 1) {
                return;
            }
        }
        return;
    } else {
        while (_time_out--) { //Signal
            if ((inb(0x64) & 2) == 0) {
                return;
            }
        }
        return;
    }
}

void mouse_write(uint8_t a_write) {
    ps2_in_wait_until_empty();
    outb(PS2_STATE_REG, 0xD4);
    ps2_in_wait_until_empty();
    outb(PS2_DATA_PORT, a_write);
}

uint8_t mouse_read() {
    ps2_out_wait_until_full();
    return inb(PS2_DATA_PORT);
}

void ps2_mouse_set_bounds(uint32_t max_x, uint32_t max_y) {
    mouse_max_x = max_x;
    mouse_max_y = max_y;
}

void ps2_mouse_preinit() {
    uint8_t status = ps2_read_configuration_byte();

    ps2_write_configuration_byte(status | 2);


    mouse_write(MOUSE_DEFAULTS);
    mouse_read(); // Acknowledge

    mouse_write(MOUSE_STREAMING_ON);
    mouse_read(); // Acknowledge

    mouse_x = 0;
    mouse_y = 0;
}

void ps2_mouse_init() {
    install_irq_handler(12, mouse_handler);
}

uint32_t mouse_get_x() {return mouse_x;}
uint32_t mouse_get_y() {return mouse_y;}
uint8_t  mouse_get_buttons() {return mouse_buttons;}
int      mouse_get_wheel() {return mouse_wheel;}
