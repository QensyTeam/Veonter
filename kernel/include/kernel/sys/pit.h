#ifndef _KERNEL_TIMER_H
#define _KERNEL_TIMER_H

#define     PIT_FREQ            1193180

#define     PIT_WRITE_LSB_MSB       0x30
#define     PIT_WRITE_COUNTER_0     0x00
#define     PIT_BINARY_MODE         0x00
#define     PIT_SQUARE_WAVE_MODE    0x06

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void timer_init();
unsigned int timer_get_uptime();
void sleep_ticks(uint32_t delay);
void sleep(uint32_t milliseconds);
void usleep(uint32_t microseconds);

#define BOOL char
#define TRUE 1
#define FALSE 0

#endif