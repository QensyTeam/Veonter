#include <kernel/kernel.h>

// Функция для отправки байта данных на порт ввода-вывода
void outb(uint16_t port, uint8_t data)
{
    // Используем инлайн-ассемблер для выполнения инструкции outb
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

// Функция для получения байта данных с порта ввода-вывода
uint8_t inb(uint16_t port)
{
    // Используем инлайн-ассемблер для выполнения инструкции inb
    uint8_t result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Функция для получения слова (16 бит) данных с порта ввода-вывода
uint16_t inw(uint16_t port)
{
    // Используем инлайн-ассемблер для выполнения инструкции inw
    uint16_t result;
    __asm__ volatile ("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}


void outw(uint16_t port, uint16_t data) {
    __asm__ volatile("out %%ax, %%dx" : : "a" (data), "d" (port));
}

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

