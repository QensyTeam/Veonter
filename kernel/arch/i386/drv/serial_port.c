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

void serial_write_int(serial_port_t port, int32_t i){
    if (i < 0) {
        serial_write_char(port, '-');
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
    serial_write_string(port,str);
}

void serial_write_uhex(serial_port_t port, uint32_t i) {
    const unsigned char hex[16]  =  { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    uint32_t n, d = 0x10000000;

    while ((i / d == 0) && (d >= 0x10)) {
        d /= 0x10;
    }
    n = i;

    while (d >= 0xF) {
        serial_write_char(port,hex[n / d]);
        n = n % d;
        d /= 0x10;
    }
    serial_write_char(port,hex[n]);
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
                serial_write_string(port, string?string:"(null)");
                break;
            case 'c':
                serial_write_char(port, (char)va_arg(args, int));
                break;
            case 'd':
                serial_write_int(port, va_arg(args, int));
                break;
            case 'i':
                serial_write_int(port, va_arg(args, int));
                break;
            case 'u':
                serial_write_int(port, va_arg(args, unsigned int));
                break;
            case 'x':
                serial_write_uhex(port, va_arg(args, uint32_t));
                break;
            case 'z': {
                i++;
                if(format[i] == 'u') {
                    serial_write_uhex(port, va_arg(args, size_t));
                    //i++;
                }
                break;
            }
            default:
                serial_write_char(port, format[i]);
            }
        } else {
            serial_write_char(port, format[i]);
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
