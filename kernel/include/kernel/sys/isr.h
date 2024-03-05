#ifndef _KERNEL_SYS_ISR_H
#define _KERNEL_SYS_ISR_H

#include <stdint.h>

typedef struct registers
{
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, useresp, ss; /* pushed by the proc automatically */
} registers_t;

typedef void (*isr_handler_routine)(registers_t r);

void fault_handler();
void install_irq_handler(int i, isr_handler_routine irq_handler);
void uninstall_irq_handler(int i);

#endif
