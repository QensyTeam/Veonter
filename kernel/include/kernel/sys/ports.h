// Файл ports.h

#ifndef _KERNEL_SYS_PORTS_H
#define _KERNEL_SYS_PORTS_H

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#define qemu_log(M, ...) __com_formatString(0x3f8,"[LOG] (%s:%s:%d) " M "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

typedef unsigned int 	u32int;
typedef          int	s32int;
typedef unsigned short	u16int;
typedef          short  s16int;
typedef unsigned char	u8int;
typedef          char	s8int;

// Функция для отправки байта данных на порт ввода-вывода
void outb(uint16_t port, uint8_t data);

// Функция для получения байта данных с порта ввода-вывода
uint8_t inb(uint16_t port);

// Функция для получения слова (16 бит) данных с порта ввода-вывода
uint16_t inw(uint16_t port);

void outw(uint16_t port, uint16_t data);
void outl(uint16_t port, uint32_t val);
uint32_t inl(uint16_t port);
void __com_setInit(uint16_t key, uint16_t value);
uint16_t __com_getInit(uint16_t key);
uint8_t __com_readByte(uint16_t port);
void __com_writeInt32(uint16_t port, uint32_t val);
uint32_t __com_readInt32(uint16_t port);
uint16_t __com_readWord(uint16_t port);
void __com_writeWord(uint16_t port, uint16_t data);
void __com_readBigData(uint16_t port, uint32_t *buffer, size_t times);
void __com_writeBigData(uint16_t port, uint32_t *buffer, size_t times);
void __com_readString(uint16_t port, uint32_t *buf, size_t size);
int32_t __com_is_ready(uint16_t port);
void __com_writeChar(uint16_t port,char a);
void __com_writeString(uint16_t port, char *buf);
void __com_io_wait();
void __com_writeInt(int16_t port, int32_t i);
void __com_writeHex(int16_t port,uint32_t i,bool mode);
void __com_pre_formatString(int16_t port, char *restrict format, va_list args);
void __com_formatString(int16_t port, char *text, ...);
int __com_init(uint16_t port);

#endif
