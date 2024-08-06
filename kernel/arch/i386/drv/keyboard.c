#include "kernel/sys/gui/shell.h"
#include <kernel/kernel.h>
#include <kernel/sys/isr.h>
#include <kernel/drv/keyboard.h>
#include <kernel/drv/tty.h>
#include <stdint.h>
#define KEYBOARD_BUFFER_SIZE 256

#define KEY_UP 0x48
#define KEY_DOWN 0x50

bool keyboard_ru = false;

static uint8_t alt_flag = 0;
static uint8_t shift_flag = 0;
static uint8_t caps_lock_flag = 0;

static uint16_t keyboard_buffer[KEYBOARD_BUFFER_SIZE] = {0};
static size_t keyboard_buffer_start = 0;
static size_t keyboard_buffer_end = 0;

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

void keyboard_handler() {
    uint8_t scancode = inb(0x60);

    if (((scancode & ~0x80) == 0x2A) || ((scancode & ~0x80) == 0x36)) {
        shift_flag = !(scancode & 0x80);

        if(alt_flag && !(scancode & 0x80)) {
            keyboard_ru = !keyboard_ru;
        }
    } else if((scancode & ~0x80) == 0x38) {
        alt_flag = !(scancode & 0x80);
    } else if (scancode == 0x3A) {
        caps_lock_flag = !caps_lock_flag;
    } else if (scancode == 0xE0) {
        // Следующий байт будет указывать на код стрелки
        uint8_t arrow_key = inb(0x60);
        if (arrow_key == KEY_UP) {
            keyboard_add_to_buffer('\x1B'); // Используем ESC в качестве специального символа
            keyboard_add_to_buffer('[');
            keyboard_add_to_buffer('A'); // Обозначение для стрелки вверх
        } else if (arrow_key == KEY_DOWN) {
            keyboard_add_to_buffer('\x1B');
            keyboard_add_to_buffer('[');
            keyboard_add_to_buffer('B'); // Обозначение для стрелки вниз
        }
    } else if (scancode < 128) {
        uint16_t c;

        if(keyboard_ru) {
            uint16_t raw_c;

            if(shift_flag ^ caps_lock_flag) {
                raw_c = shifted_keyboard_layout_ru[scancode];
            } else {
                raw_c = keyboard_layout_ru[scancode];
            }

            // In strings we have a deal with ENCODED UNICODE characters
            // But when we're working with characters, it seems we having deal with raw codes.
        
            c = raw_c;
            if(c != keyboard_layout[scancode]) {
                c = codepoint_to_utf8_short(raw_c);
            }

        } else {
            if(shift_flag ^ caps_lock_flag) {
                c = shifted_keyboard_layout[scancode];
            } else {
                c = keyboard_layout[scancode];
            }
        }

        if (c != 0) {
            shell_putchar(c); // Выводим символ на экран
            keyboard_add_to_buffer(c); // Добавляем символ в буфер
        }
    }

    pic_eoi(KEYBOARD_IRQ); // Отправляем сигнал "конец прерывания"
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

void keyboard_init() {
    install_irq_handler(KEYBOARD_IRQ, keyboard_handler);
}
