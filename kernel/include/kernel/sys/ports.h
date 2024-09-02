// Файл ports.h

#ifndef _KERNEL_SYS_PORTS_H
#define _KERNEL_SYS_PORTS_H

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

void outl(uint16_t port, uint32_t val);
uint32_t inl(uint16_t port);

// Функция для отправки байта данных на порт ввода-вывода
__attribute__((always_inline))
inline void outb(uint16_t port, uint8_t data)
{
    // Используем инлайн-ассемблер для выполнения инструкции outb
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

// Функция для получения байта данных с порта ввода-вывода
__attribute__((always_inline))
inline uint8_t inb(uint16_t port) {
    // Используем инлайн-ассемблер для выполнения инструкции inb
    uint8_t result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Функция для получения слова (16 бит) данных с порта ввода-вывода
__attribute__((always_inline))
inline uint16_t inw(uint16_t port)
{
    // Используем инлайн-ассемблер для выполнения инструкции inw
    uint16_t result;
    __asm__ volatile ("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

__attribute__((always_inline))
inline void outw(uint16_t port, uint16_t data) {
    __asm__ volatile ("outw %1, %0" :: "Nd" (port), "a" (data));
    //__asm__ volatile ("outw %0, %1" :: "a" (port), "Nd" (data));
}


#endif
