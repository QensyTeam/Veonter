#include <kernel/sys/isr.h>
#include <kernel/sys/pic.h>
#include <stddef.h>
#include <kernel/panic.h>
#include <stdio.h>
#include <kernel/drv/tty.h>

isr_handler_routine irq_handlers[PIC_NUM_INTERRUPTS] = {0}; 

void isr_handler(__attribute__ ((unused)) registers_t* r) {
    PANIC("exception received, don't recover yet");
}

void install_irq_handler(int i, isr_handler_routine irq_handler) {
    irq_handlers[i] = irq_handler;
}

void uninstall_irq_handler(int i) {
    irq_handlers[i] = NULL;
}

void irq_server(registers_t* r) {
    isr_handler_routine irq_handler;
    int irq_num = r->int_no - 32;
    irq_handler = irq_handlers[irq_num];

    if (irq_handler != NULL) {
        irq_handler(*r); 
    }

    pic_eoi(r->int_no);  // Отправляем сигнал "конец прерывания"
}
