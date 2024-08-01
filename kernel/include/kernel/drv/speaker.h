#pragma once

#include <kernel/sys/ports.h>

void play_sound(uint32_t nFrequence);
void nosound();
void beep(int frequency, int duration);
void sleep_s(int duration);