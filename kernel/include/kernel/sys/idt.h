#ifndef _KERNEL_SYS_IDT_H
#define _KERNEL_SYS_IDT_H

#include <stdint.h>

#define MAX_INTERRUPTS		256

#define IDT_DESC_BIT16		0x06	//00000110
#define IDT_DESC_BIT32		0x0E	//00001110
#define IDT_DESC_RING1		0x40	//01000000
#define IDT_DESC_RING2		0x20	//00100000
#define IDT_DESC_RING3		0x60	//01100000
#define IDT_DESC_PRESENT	0x80	//10000000

struct __attribute__ ((__packed__)) idt_descriptor {
	uint16_t		offset_low;
	uint16_t		sel;
	uint8_t			reserved;
	uint8_t			flags;
	uint16_t		offset_high;
};

struct __attribute__((__packed__)) idtr {
	uint16_t		limit;
	uint32_t		base;
};

void idt_init(uint16_t codeSel);
void idt_set_entry (uint32_t i, uint16_t flags, uint16_t sel, void (*irq)());

#endif