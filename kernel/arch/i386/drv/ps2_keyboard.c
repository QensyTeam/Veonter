#include <kernel/kernel.h>
#include <kernel/sys/isr.h>
#include <kernel/drv/ps2_keyboard.h>
#include <kernel/drv/tty.h>
#include <stdint.h>
#include <kernel/drv/ps2.h>

#define KEY_UP 0x48
#define KEY_DOWN 0x50
#define KEY_LEFT  0x4B
#define KEY_RIGHT 0x4D

static volatile bool keyboard_ru = false;

static volatile uint8_t alt_flag = 0;
static volatile uint8_t shift_flag = 0;
static volatile uint8_t caps_lock_flag = 0;



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
        } else if (arrow_key == KEY_LEFT) {
            keyboard_add_to_buffer('\x1B');
            keyboard_add_to_buffer('[');
            keyboard_add_to_buffer('D'); // Обозначение для стрелки влево
        } else if (arrow_key == KEY_RIGHT) {
            keyboard_add_to_buffer('\x1B');
            keyboard_add_to_buffer('[');
            keyboard_add_to_buffer('C'); // Обозначение для стрелки вправо
        }
    } else if (scancode < 128) {
        uint16_t c;
        uint16_t base = keyboard_layout[scancode];

        bool is_letter = (base >= 'a' && base <= 'z');
        
        bool shift_letters = caps_lock_flag ^ shift_flag;
        bool shift_numbers = shift_flag;

        bool shift = is_letter ? shift_letters : shift_numbers;

        // Caps Shift Key Shifted
        // 0    0     L   0
        // 1    0     L   1
        // 0    1     L   1
        // 1    1     L   0
        
        // 0    0     S   0
        // 1    0     S   0
        // 0    1     S   1
        // 1    1     S   1

        if (keyboard_ru) {
            uint16_t raw_c;
            if (shift) {
                raw_c = shifted_keyboard_layout_ru[scancode];
            } else {
                raw_c = keyboard_layout_ru[scancode];
            }

            c = raw_c;
            if(c != keyboard_layout[scancode]) {
                c = codepoint_to_utf8_short(raw_c);
            }
        } else {
            if (shift) {
                c = shifted_keyboard_layout[scancode];
            } else {
                c = keyboard_layout[scancode];
            }
        }

        if (c != 0) {
            keyboard_add_to_buffer(c); // Добавляем символ в буфер
        }
    }
}


void ps2_keyboard_preinit() {
    uint8_t stat;

    // Enable keyboard
    ps2_write(0xf4);
    stat = ps2_read();

    if(stat != 0xfa) {
        printf("Kbd error: LN %u\n", __LINE__);
        return;
    }

    // Set scancode
    ps2_write(0xf0);
    stat = ps2_read();

    if(stat != 0xfa) {
        printf("Kbd error: LN %u\n", __LINE__);
        return;
    }

    // Scancode number 0! (IDK what does it mean)
    ps2_write(0);
    stat = ps2_read();

    if(stat != 0xfa) {
        printf("Kbd error: LN %u\n", __LINE__);
        return;
    }

    // With error code PS/2 controller wants to send us scancode set number! Awwwwwww
    uint8_t scancode = ps2_read();
    (void)scancode;  // Consume it 

    ps2_write(0xf3);
    stat = ps2_read();

    if(stat != 0xfa) {
        printf("Kbd error: LN %u (rpt = %u)\n", __LINE__, stat);
        return;
    }

    ps2_in_wait_until_empty();

    outb(PS2_DATA_PORT, 0);
    stat = ps2_read();

    if(stat != 0xfa) {
        printf("Kbd error: LN %u\n", __LINE__);
        return;
    }

    uint8_t conf = ps2_read_configuration_byte();

    ps2_write_configuration_byte(conf | 0b1000001);
}

void ps2_keyboard_init() {
    install_irq_handler(KEYBOARD_IRQ, keyboard_handler);
}
