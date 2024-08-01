#include <kernel/sys/pit.h>
#include <kernel/sys/isr.h>
#include <kernel/sys/ports.h>
#include <stdint.h>
#include <stdio.h>
#include <kernel/kernel.h>
#include <stddef.h>

#define PIT_REG_COUNTER_0   0x40
#define PIT_REG_COMMAND     0x43
#define TIMER_FREQUENCY     100  // Частота таймера в Гц

unsigned int ticks = 0;
BOOL should_run = TRUE;
extern bool cursor_visible;

void timer_irq_handler(__attribute__ ((unused)) registers_t r) {
    ticks++;
    if (ticks % (TIMER_FREQUENCY / 2) == 0) { // курсор мигает раз в 0.5 секунды
        if (cursor_visible) {
            disable_cursor();
        } else {
            enable_cursor();
        }
        cursor_visible = !cursor_visible; // переключаем видимость
    }
}


void sleep_ticks(uint32_t delay_ticks) {
    size_t start_ticks = ticks;

    while (ticks - start_ticks < delay_ticks) {
        //занят
    }
}

void sleep(uint32_t milliseconds) {
    uint32_t needticks = milliseconds * TIMER_FREQUENCY;

    sleep_ticks(needticks / 1000);
}

void usleep(uint32_t microseconds) {
    uint32_t delay_ticks = (microseconds * TIMER_FREQUENCY) / 1000000;
    sleep_ticks(delay_ticks);
}

unsigned int timer_get_uptime() {
    return ticks;
}

void timer_set_phase(uint32_t hz) {
    int div = PIT_FREQ / hz;
    uint8_t ocw = PIT_WRITE_LSB_MSB | PIT_WRITE_COUNTER_0 |
                  PIT_BINARY_MODE   | PIT_SQUARE_WAVE_MODE;

    outb(PIT_REG_COMMAND, ocw);
    outb(PIT_REG_COUNTER_0, (div & 0xFF));
    outb(PIT_REG_COUNTER_0, (div >> 8) & 0xFF);
}

void timer_init() {
    timer_set_phase(TIMER_FREQUENCY);
    install_irq_handler(0, timer_irq_handler);
    check();
    printf("PIT initialization completed successfully!\n");
}
