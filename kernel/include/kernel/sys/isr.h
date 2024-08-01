#ifndef _KERNEL_SYS_ISR_H
#define _KERNEL_SYS_ISR_H

#include <stdint.h>

typedef struct registers
{
    unsigned int gs, fs, es, ds;      
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  
    unsigned int int_no, err_code;    
    unsigned int eip, cs, eflags, useresp, ss; 
} registers_t;

typedef void (*isr_handler_routine)(registers_t r);

void install_irq_handler(int i, isr_handler_routine irq_handler);
void uninstall_irq_handler(int i);

#endif
