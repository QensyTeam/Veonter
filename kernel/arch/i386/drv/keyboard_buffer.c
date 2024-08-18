#include <kernel/drv/keyboard_buffer.h>
#include <stddef.h>

static volatile uint16_t keyboard_buffer[KEYBOARD_BUFFER_SIZE] = {0};
static volatile size_t keyboard_buffer_start = 0;
static volatile size_t keyboard_buffer_end = 0;

// TODO: Move it into unicode.c or make it static and move it into unicode.h
uint16_t codepoint_to_utf8_short(uint16_t code) {
    if(code <= 0xff) {
        return code & 0b01111111;
    }

    uint32_t low = (code & 0b111111) | 0b10000000;
    uint32_t hi = ((code >> 6) & 0b11111) | 0b11000000;

    //return (hi << 8) | low;
    return (low << 8) | hi;
}

// Добавляем символ в буфер клавиатуры
void keyboard_add_to_buffer(uint16_t c) {
    size_t next = (keyboard_buffer_end + 1) % KEYBOARD_BUFFER_SIZE;
    if (next != keyboard_buffer_start) { // Проверка на переполнение буфера
        keyboard_buffer[keyboard_buffer_end] = c;
        keyboard_buffer_end = next;
    }
}

// Получаем символ из буфера клавиатуры
uint16_t keyboard_get_from_buffer() {
    if (keyboard_buffer_start == keyboard_buffer_end) {
        return '\0'; // Если буфер пуст, возвращаем нулевой символ
    } else {
        uint16_t c = keyboard_buffer[keyboard_buffer_start];
        keyboard_buffer_start = (keyboard_buffer_start + 1) % KEYBOARD_BUFFER_SIZE;
        return c;
    }
}

// Функция для получения символа из буфера
uint16_t keyboard_get_char() {
    uint16_t c;
    // Ждем, пока буфер не будет пуст
    while ((c = keyboard_get_from_buffer()) == 0) {
        // Здесь может быть добавлено ожидание (idle), чтобы не нагружать процессор
    }
    return c;
}

