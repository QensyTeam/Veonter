#include <kernel/sys/pit.h>
#include <kernel/sys/isr.h>
#include <kernel/sys/ports.h>
#include <stdint.h>
#include <stdio.h>
#include <kernel/drv/tty.h>
#include <stddef.h>

#define PIT_REG_COUNTER_0   0x40
#define PIT_REG_COMMAND     0x43
#define TIMER_FREQUENCY     100  // Частота таймера в Гц
#define SECONDS_TO_RUN      10   // Количество секунд, в течение которых будет выводиться сообщение

unsigned int ticks = 0;
BOOL should_run = TRUE;

void timer_irq_handler(__attribute__ ((unused)) registers_t r) {
    ticks++;
    /*static uint32_t milliseconds = 0;
    static uint32_t seconds = 0;
    static uint32_t minutes = 0;

    
    milliseconds += 10; // Увеличиваем количество миллисекунд на 10 при каждом прерывании

    if (milliseconds >= 1000) {
        milliseconds = 0;
        seconds++;

        if (seconds >= 60) {
            seconds = 0;
            minutes++;
        }
    }

    printf("Time: %d:%d:%d\n", minutes, seconds, milliseconds);
    
    if (minutes >= 0 && seconds >= SECONDS_TO_RUN) {
        should_run = FALSE;
    }*/
}

void sleep_ticks(uint32_t delay){
	size_t current_ticks = ticks;

	while(1) {
		if (current_ticks + delay < ticks){
			break;
		}
	}

}

void sleep(uint32_t milliseconds) {
	uint32_t needticks = milliseconds * TIMER_FREQUENCY;

	sleep_ticks(needticks / 1000);
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