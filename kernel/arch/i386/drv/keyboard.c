#include <kernel/sys/pic.h>
#include <kernel/sys/isr.h>
#include <kernel/drv/keyboard.h>
#include <kernel/drv/tty.h>
#define KEYBOARD_BUFFER_SIZE 128
// Определяем английскую раскладку клавиатуры
static const char keyboard_layout[128] = {
    0,   27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,   'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,   '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0,   '*', 0, ' ', 0,   0,   0,   0,   0,   0,   0,
    0,   0
};

// Определяем таблицу символов для клавиш при зажатой клавише Shift
static const char shifted_keyboard_layout[128] = {
    0,   27,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,   'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,   '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', 0,   '*', 0, ' ', 0,   0,   0,   0,   0,   0,   0,
    0,   0
};

static const char caps_locked_keyboard_layout[128] = {
    0,   27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n', 0,   'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0,   '\\', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', ',', '.', '/', 0,   '*', 0, ' ', 0,   0,   0,   0,   0,   0,   0,
    0,   0
};

static const char caps_locked_shifted_keyboard_layout[128] = {
    0,   27,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\n', 0,   'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '"', '~', 0,   '|', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', '<', '>', '?', 0,   '*', 0, ' ', 0,   0,   0,   0,   0,   0,   0,
    0,   0
};

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
            terminal_putchar(c); // Выводим символ на экран
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