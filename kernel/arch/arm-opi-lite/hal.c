// ARM code for Veonter by NDRAEY (c) 2024


#include <stdint.h>
#include <stddef.h>
#include <kernel/drv/serial_port.h>
#include <stdarg.h>

#define UART 0x01c28000

volatile size_t ticks = 0;

static inline uint8_t uart_tx_rdy() {
	return (uint8_t)((*(volatile uint32_t*)(UART + 0x14)) & 0x40);
}

void uart_write_byte(size_t addr, char byte) {
  while (!uart_tx_rdy())
    ;
    
  *(volatile uint32_t*)(addr) = byte;
}

void uart_write_string(size_t addr, char* string) {
	while(*string) {
        uart_write_byte(addr, *string);
        string++;
    };
}

void uart_write_int(serial_port_t port, int32_t i){
    if (i < 0) {
        uart_write_byte(port, '-');
        i = -i;
    }
    
    uint32_t n, d = 1000000000;
    char str[255];
    uint32_t dec_index = 0;

    while ((i / d == 0) && (d >= 10)) {
        d /= 10;
    }
    n = i;

    while (d >= 10) {
        str[dec_index++] = ((char) ((int) '0' + n / d));
        n = n % d;
        d /= 10;
    }

    str[dec_index++] = ((char) ((int) '0' + n));
    str[dec_index] = 0;
    uart_write_string(port, str);
}

void uart_write_uhex(serial_port_t port, uint32_t i) {
    const unsigned char hex[16]  =  { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    uint32_t n, d = 0x10000000;

    while ((i / d == 0) && (d >= 0x10)) {
        d /= 0x10;
    }
    n = i;

    while (d >= 0xF) {
        uart_write_byte(port,hex[n / d]);
        n = n % d;
        d /= 0x10;
    }
    uart_write_byte(port,hex[n]);
}

void serial_vprintf(serial_port_t port, char *restrict format, va_list args) {
    uint32_t i = 0;
    char *string;

    while (format[i]) {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
            case 's':
                string = va_arg(args, char*);
                uart_write_string(port, string?string:"(null)");
                break;
            case 'c':
                uart_write_byte(port, (char)va_arg(args, int));
                break;
            case 'd':
                uart_write_int(port, va_arg(args, int));
                break;
            case 'i':
                uart_write_int(port, va_arg(args, int));
                break;
            case 'u':
                uart_write_int(port, va_arg(args, unsigned int));
                break;
            case 'x':
                uart_write_uhex(port, va_arg(args, uint32_t));
                break;
            case 'z': {
                i++;
                if(format[i] == 'u') {
                    uart_write_uhex(port, va_arg(args, size_t));
                    //i++;
                }
                break;
            }
            default:
                uart_write_byte(port, format[i]);
            }
        } else {
            uart_write_byte(port, format[i]);
        }
        i++;
    }
}

void serial_printf(serial_port_t port, char *text, ...) {
    va_list args;
    va_start(args, text);
    
    serial_vprintf(port, text, args);
    
    va_end(args);
}

void shell_putchar(char ch) {
    uart_write_byte(UART, ch);
}

void console_input_loop() {

}

__attribute__((noreturn)) void init_hal() {
	uart_write_string(UART, "Hello, Veronika Enter!\n\r");

	while(1) {}
}
