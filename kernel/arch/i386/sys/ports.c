#include <kernel/kernel.h>

uint16_t com_init[8] = {0};    ///< Массив с инициализированными портами
// Функция для отправки байта данных на порт ввода-вывода
void outb(uint16_t port, uint8_t data)
{
    // Используем инлайн-ассемблер для выполнения инструкции outb
    asm volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

// Функция для получения байта данных с порта ввода-вывода
uint8_t inb(uint16_t port)
{
    // Используем инлайн-ассемблер для выполнения инструкции inb
    uint8_t result;
    asm volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Функция для получения слова (16 бит) данных с порта ввода-вывода
uint16_t inw(uint16_t port)
{
    // Используем инлайн-ассемблер для выполнения инструкции inw
    uint16_t result;
    asm volatile ("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}


void outw(uint16_t port, uint16_t data) {
    asm("out %%ax, %%dx" : : "a" (data), "d" (port));
}

void outl(uint16_t port, uint32_t val) {
    asm volatile ( "outl %0, %1" : : "a"(val), "Nd"(port) );
}

uint32_t inl(uint16_t port) {
    uint32_t ret;
    asm volatile ( "inl %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

void irq_disable() {
    __asm__( "cli" );
}

void irq_enable() {
    __asm__( "sti" );
}

void __com_setInit(uint16_t key, uint16_t value){
    com_init[key] = value;
}

uint16_t __com_getInit(uint16_t key){
    return com_init[key];
}

uint8_t __com_readByte(uint16_t port){
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

void __com_writeByte(uint16_t port, uint8_t val){
    asm volatile("outb %1, %0" 
    : 
    : "dN"(port), 
      "a"(val)
    );
}

void __com_writeInt32(uint16_t port, uint32_t val) {
    asm volatile ( "outl %0, %1" : : "a"(val), "Nd"(port) );
}

uint32_t __com_readInt32(uint16_t port) {
    uint32_t ret;
    asm volatile ( "inl %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

uint16_t __com_readWord(uint16_t port) {
    uint16_t rv;
    asm volatile ("inw %1, %0" : "=a" (rv) : "dN" (port));
    return rv;
}

void __com_writeWord(uint16_t port, uint16_t data) {
    asm volatile ("outw %1, %0" : : "dN" (port), "a" (data));
}

void __com_readBigData(uint16_t port, uint32_t *buffer, size_t times) {
    for (size_t index = 0; index < times; index++) {
        buffer[index] = inl(port);
    }
}

void __com_writeBigData(uint16_t port, uint32_t *buffer, size_t times) {
    for (size_t index = 0; index < times; index++) {
        outl(port, buffer[index]);
    }
}

void __com_readString(uint16_t port, uint32_t *buf, size_t size){
    __com_readBigData(port, buf, size);
}

int32_t __com_is_ready(uint16_t port){
    return __com_readByte(port + 5) & 0x20;
}

void __com_writeChar(uint16_t port,char a) {
    while (__com_is_ready(port) == 0);
    outb(port, a);
}

void __com_writeString(uint16_t port, char *buf){
    for (size_t i = 0, len = strlen(buf); i < len; i++) {
        __com_writeChar(port,buf[i]);
    }
}

void __com_io_wait(){
    outb(0x80, 0);
}


void __com_writeInt(int16_t port, int32_t i){
    if (i < 0) {
        __com_writeChar(port,'-');
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
    __com_writeString(port,str);
}

void __com_writeHex(int16_t port,uint32_t i,bool mode){
    const unsigned char hex[16]  =  { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    uint32_t n, d = 0x10000000;

    if (mode) __com_writeString(port,"0x");

    while ((i / d == 0) && (d >= 0x10)) {
        d /= 0x10;
    }
    n = i;

    while (d >= 0xF) {
        __com_writeChar(port,hex[n / d]);
        n = n % d;
        d /= 0x10;
    }
    __com_writeChar(port,hex[n]);
}


void __com_pre_formatString(int16_t port, char *restrict format, va_list args){
    int32_t i = 0;
    char *string;

    while (format[i]) {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
            case 's':
                string = va_arg(args, char*);
                __com_writeString(port, string?string:"(null)");
                break;
            case 'c':
                __com_writeChar(port,(char)va_arg(args, int));
                break;
            case 'd':
                __com_writeInt(port,va_arg(args, int));
                break;
            case 'i':
                __com_writeInt(port,va_arg(args, int));
                break;
            case 'u':
                __com_writeInt(port,va_arg(args, unsigned int));
                break;
            case 'x':
                __com_writeHex(port,va_arg(args, uint32_t),true);
                break;
            case 'v':
                __com_writeHex(port,va_arg(args, uint32_t),false);
                break;
            default:
                __com_writeChar(port,format[i]);
            }
        } else {
            __com_writeChar(port,format[i]);
        }
        i++;
    }
}

void __com_formatString(int16_t port, char *text, ...) {
    va_list args;
    va_start(args, text);
     __com_pre_formatString(port,text, args);
    va_end(args);
}

int __com_init(uint16_t port) {
    outb(port + 1, 0x00);    
    outb(port + 3, 0x80);    
    outb(port + 0, 0x01);    
    outb(port + 1, 0x00);    
    outb(port + 3, 0x03);    
    outb(port + 2, 0xC7);    
    outb(port + 4, 0x0B);    
    outb(port + 4, 0x1E);   
    outb(port + 0, 0xAE);    

    if(inb(port + 0) != 0xAE) {
        return 1;
    }
    outb(port + 4, 0x0F);

    return 0;
}
