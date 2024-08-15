#pragma once

#include <stdint.h>

__attribute__((always_inline))
inline uint8_t bit_flip_byte(uint8_t byte, int num_bits) {
    uint8_t x = byte << (8 - num_bits);
    return x | (byte >> num_bits);
}

__attribute__((always_inline))
inline uint16_t bit_flip_short(uint16_t short_int) {
    uint32_t first_byte = *((uint8_t*)(&short_int));
    uint32_t second_byte = *((uint8_t*)(&short_int) + 1);
    return (first_byte << 8) | (second_byte);
}

__attribute__((always_inline))
inline uint32_t bit_flip_int(uint32_t long_int) {
    uint32_t first_byte = *((uint8_t*)(&long_int));
    uint32_t second_byte = *((uint8_t*)(&long_int) + 1);
    uint32_t third_byte = *((uint8_t*)(&long_int)  + 2);
    uint32_t fourth_byte = *((uint8_t*)(&long_int) + 3);
    return (first_byte << 24) | (second_byte << 16) | (third_byte << 8) | (fourth_byte);
}
