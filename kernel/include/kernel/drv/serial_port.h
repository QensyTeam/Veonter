#pragma once

extern volatile unsigned int ticks;

#define qemu_log(M, ...) serial_printf(0x3f8, "\x1b[1m[LOG %d.%d] (%s:%s:%d) " M "\x1b[0m\n", ticks / 100, ticks % 100, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

typedef enum {
    COM1 = 0x3f8,
	COM2 = 0x2F8,
	COM3 = 0x3E8,
	COM4 = 0x2E8,
	COM5 = 0x5F8,
	COM6 = 0x4F8,
	COM7 = 0x5E8,
	COM8 = 0x4E8,
} serial_port_t;

void serial_write_char(serial_port_t port, char ch);
void serial_write_string(serial_port_t port, const char* str);
void serial_port_init(serial_port_t port);
void serial_printf(serial_port_t port, char *text, ...);

