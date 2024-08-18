#pragma once

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

