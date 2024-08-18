#include <kernel/drv/serial_port.h>
#include <kernel/sys/ports.h>

__attribute__((always_inline))
inline bool serial_is_empty(serial_port_t port) {
    return inb(port + 5) & 0x20;
}

void serial_write_char(serial_port_t port, char ch) {
    outb((uint16_t)port, ch);
}

void serial_write_string(serial_port_t port, const char* str) {
    do {
        serial_write_char(port, *str);
    } while(*str++); 
}

void serial_port_init(serial_port_t port) {
    outb(port + 1, 0x00);    
    outb(port + 3, 0x80);    
    outb(port + 0, 0x03);    
    outb(port + 1, 0x00);    
    outb(port + 3, 0x03);    
    outb(port + 2, 0xC7);    
    outb(port + 4, 0x0B);    
    outb(port + 4, 0x1E);    
    outb(port + 0, 0xAE);    

    if(inb(port + 0) != 0xAE) {
        return;
    }
        
    outb(port + 4, 0x0F);
}
