#include <kernel/sys/pic.h>
#include <kernel/sys/isr.h>
#include <kernel/drv/keyboard.h>
#include <kernel/drv/tty.h>

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

void keyboard_handler() {
    // Считываем код клавиши из порта ввода-вывода (порт 0x60)
    uint8_t scancode = inb(0x60);

    // Обрабатываем клавиши Shift и Caps Lock
    if (scancode == 0x2A || scancode == 0x36) {
        shift_flag = (scancode == 0x2A) ? 1 : 0;
    } else if (scancode == 0x3A) {
        caps_lock_flag = !caps_lock_flag;
    } else if (scancode < 128) {
        // Если у нас есть обработчик для данного кода клавиши, выводим символ на экран
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
            terminal_putchar(c);
        }
    }

    // Сбрасываем флаг shift_flag, когда клавиша Shift отпущена
    if (scancode == 0xAA || scancode == 0xB6) {
        shift_flag = 0;
    }

    // Обязательно отправляем сигнал "конец прерывания" (End of Interrupt) контроллеру прерываний
    pic_eoi(KEYBOARD_IRQ);
}

void keyboard_init() {
    // Установим обработчик прерывания для клавиатуры.
    install_irq_handler(KEYBOARD_IRQ, keyboard_handler);
}
