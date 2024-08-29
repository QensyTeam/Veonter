#include <kernel/kernel.h>

void outl(uint16_t port, uint32_t val) {
    __asm__ volatile ( "outl %0, %1" : : "a"(val), "Nd"(port) );
}

uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile ( "inl %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

void irq_disable() {
    __asm__ volatile( "cli" );
}

void irq_enable() {
    __asm__ volatile( "sti" );
}

