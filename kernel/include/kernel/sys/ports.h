// Файл ports.h

#ifndef _KERNEL_SYS_PORTS_H
#define _KERNEL_SYS_PORTS_H

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

typedef unsigned int 	u32int;
typedef          int	s32int;
typedef unsigned short	u16int;
typedef          short  s16int;
typedef unsigned char	u8int;
typedef          char	s8int;

// Функция для получения байта данных с порта ввода-вывода
uint8_t inb(uint16_t port);

// Функция для получения слова (16 бит) данных с порта ввода-вывода
uint16_t inw(uint16_t port);

void outw(uint16_t port, uint16_t data);
void outl(uint16_t port, uint32_t val);
uint32_t inl(uint16_t port);

// Функция для отправки байта данных на порт ввода-вывода
__attribute__((always_inline))
inline void outb(uint16_t port, uint8_t data)
{
    // Используем инлайн-ассемблер для выполнения инструкции outb
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

#endif
