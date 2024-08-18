#pragma once

#include <stdint.h>

#define KEYBOARD_BUFFER_SIZE 256

uint16_t keyboard_get_char();
uint16_t keyboard_get_from_buffer();
void keyboard_add_to_buffer(uint16_t c);
uint16_t codepoint_to_utf8_short(uint16_t code);
