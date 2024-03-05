#ifndef _KERNEL_DRV_KEYBOARD_H
#define _KERNEL_DRV_KEYBOARD_H

#include <kernel/sys/ports.h>
#include <kernel/sys/isr.h>

#define KEYBOARD_IRQ 1

void keyboard_handler();
void keyboard_init();

#endif 