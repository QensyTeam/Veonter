#include <kernel/sys/pic.h>
#include <kernel/sys/ports.h>
#include <stdint.h>
#include <stdio.h>
#include <kernel/drv/tty.h>

#define PIC1		    0x20		/* IO base address for master PIC */
#define PIC2		    0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	   (PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	   (PIC2+1)

#define ICW1_ICW4_EXPECT	0x01		/* ICW4 (not) needed */
#define ICW1_INIT	        0x10		/* Initialization - required! */
#define ICW4_8086	        0x01		/* 8086/88 (MCS-80/85) mode */
 
#define PIC1_IRQ_OFFSET 0x20
#define PIC2_IRQ_OFFSET 0x28

// TODO: add io_wait between writes to PIC
void pic_init(void) {
    uint8_t icw = ICW1_INIT | ICW1_ICW4_EXPECT;

	outb(PIC1_COMMAND, icw);      // starts initialization sequence (in cascade mode)
	outb(PIC2_COMMAND, icw);

	outb(PIC1_DATA, PIC1_IRQ_OFFSET);  // ICW2: Master PIC vector offset
	outb(PIC2_DATA, PIC2_IRQ_OFFSET);  // ICW2: Slave PIC vector offset
	
    outb(PIC1_DATA, 4); // ICW3: tell Master PIC that slave PIC at IRQ2 (0000 0100)
	outb(PIC2_DATA, 2); // ICW3: tell Slave PIC its cascade identity (0000 0010)
 
	outb(PIC1_DATA, ICW4_8086);
	outb(PIC2_DATA, ICW4_8086);

    check();
    printf("PIC initialization completed successfully!\n");
}

// Sends the correct End Of Interrupt command to the PIC
void pic_eoi(unsigned int int_no) {
    if (int_no >= 40) {
        outb(PIC2, 0x20);
    }
    outb(PIC1, 0x20);
}
