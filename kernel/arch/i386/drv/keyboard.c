#include <kernel/kernel.h>
#include <kernel/sys/isr.h>
#include <kernel/drv/keyboard.h>
#include <kernel/drv/tty.h>
#define KEYBOARD_BUFFER_SIZE 128

#define KEY_UP 0x48
#define KEY_DOWN 0x50

static uint8_t shift_flag = 0;
static uint8_t caps_lock_flag = 0;

static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static size_t keyboard_buffer_start = 0;
static size_t keyboard_buffer_end = 0;

// Добавляем символ в буфер клавиатуры
void keyboard_add_to_buffer(char c) {
    size_t next = (keyboard_buffer_end + 1) % KEYBOARD_BUFFER_SIZE;
    if (next != keyboard_buffer_start) { // Проверка на переполнение буфера
        keyboard_buffer[keyboard_buffer_end] = c;
        keyboard_buffer_end = next;
    }
}

// Получаем символ из буфера клавиатуры
char keyboard_get_from_buffer() {
    if (keyboard_buffer_start == keyboard_buffer_end) {
        return '\0'; // Если буфер пуст, возвращаем нулевой символ
    } else {
        char c = keyboard_buffer[keyboard_buffer_start];
        keyboard_buffer_start = (keyboard_buffer_start + 1) % KEYBOARD_BUFFER_SIZE;
        return c;
    }
}

void keyboard_handler() {
    uint8_t scancode = inb(0x60);

    if (scancode == 0x2A || scancode == 0x36) {
        shift_flag = 1;
    } else if (scancode == 0xAA || scancode == 0xB6) {
        shift_flag = 0;
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
        char c;

        if (shift_flag && caps_lock_flag) {
            c = caps_locked_shifted_keyboard_layout[scancode];
        } else if (shift_flag) {
            c = shifted_keyboard_layout[scancode];
        } else if (caps_lock_flag) {
            c = caps_locked_keyboard_layout[scancode];
        } else {
            c = keyboard_layout[scancode];
        }

        if (c != 0) {
            shell_putchar(c); // Выводим символ на экран
            keyboard_add_to_buffer(c); // Добавляем символ в буфер
        }
    }

    pic_eoi(KEYBOARD_IRQ); // Отправляем сигнал "конец прерывания"
}


// Функция для получения символа из буфера
char keyboard_get_char() {
    char c;
    // Ждем, пока буфер не будет пуст
    while ((c = keyboard_get_from_buffer()) == '\0') {
        // Здесь может быть добавлено ожидание (idle), чтобы не нагружать процессор
    }
    return c;
}

void keyboard_init() {
    install_irq_handler(KEYBOARD_IRQ, keyboard_handler);
}