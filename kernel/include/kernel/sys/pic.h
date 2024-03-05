#ifndef _KERNEL_SYS_PIC_H
#define _KERNEL_SYS_PIC_H

#include <stdint.h>

#define PIC_NUM_INTERRUPTS 16

void pic_init();
void pic_eoi(unsigned int int_no);

#endif
