#include <kernel/sys/pic.h>
#include <kernel/sys/ports.h>
#include <stdint.h>
#include <stdio.h>
#include <kernel/drv/tty.h>

#define PIC1		    0x20		
#define PIC2		    0xA0		
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	   (PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	   (PIC2+1)

#define ICW1_ICW4_EXPECT	0x01		
#define ICW1_INIT	        0x10		
#define ICW4_8086	        0x01		
 
#define PIC1_IRQ_OFFSET 0x20
#define PIC2_IRQ_OFFSET 0x28

void pic_init(void) {
    uint8_t icw = ICW1_INIT | ICW1_ICW4_EXPECT;

	outb(PIC1_COMMAND, icw);      
	outb(PIC2_COMMAND, icw);

	outb(PIC1_DATA, PIC1_IRQ_OFFSET);  
	outb(PIC2_DATA, PIC2_IRQ_OFFSET);  
	
    outb(PIC1_DATA, 4); 
	outb(PIC2_DATA, 2); 
 
	outb(PIC1_DATA, ICW4_8086);
	outb(PIC2_DATA, ICW4_8086);

    check();
    printf("PIC initialization completed successfully!\n");
}

void pic_eoi(unsigned int int_no) {
    if (int_no >= 40) {
        outb(PIC2, 0x20);
    }
    outb(PIC1, 0x20);
}
