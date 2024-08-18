#pragma once

#include <stdint.h>

#define MOUSE_RESET           0xFF
#define MOUSE_RESEND          0xFE
#define MOUSE_DEFAULTS        0xF6
#define MOUSE_STREAMING_OFF   0xF5
#define MOUSE_STREAMING_ON    0xF4
#define MOUSE_SET_SAMPLE_RATE 0xF3
#define MOUSE_GET_ID          0xF2
#define MOUSE_REQUEST_PACKET  0xEB
#define MOUSE_REQUEST_STATUS  0xE9
#define MOUSE_SET_RESOLUTION  0xE8
#define MOUSE_CONTROL_PORT    0x64
#define MOUSE_CMD_SEND        0xD4
#define MOUSE_DATA_PORT       0x60

#define MOUSE_BUTTON_LEFT     (1 << 0U)
#define MOUSE_BUTTON_RIGHT    (1 << 1U)
#define MOUSE_BUTTON_MIDDLE   (1 << 2U)
#define MOUSE_BUTTON_4        (1 << 3U)
#define MOUSE_BUTTON_5        (1 << 4U)

#define MOUSE_ACK             0xFA

typedef struct mouse_flags_byte {
    unsigned int left_button   : 1;
    unsigned int right_button  : 1;
    unsigned int middle_button : 1;

    unsigned int always1 : 1;

    unsigned int x_sign : 1;
    unsigned int y_sign : 1;

    unsigned int x_overflow : 1;
    unsigned int y_overflow : 1;
} __attribute__((packed)) mouse_flags_byte;

/*struct dev_ps2m_mouse_packet {
    int16_t movement_x;
    int16_t movement_y;
    uint8_t button_l;
    uint8_t button_m;
    uint8_t button_r;
} ps2m_buffer;
*/

uint32_t mouse_get_x();
uint32_t mouse_get_y();
uint8_t  mouse_get_buttons();

void ps2_mouse_set_bounds(uint32_t x, uint32_t y);

void ps2_mouse_preinit();
void ps2_mouse_init();
