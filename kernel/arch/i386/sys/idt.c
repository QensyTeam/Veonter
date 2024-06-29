
#include <stdio.h>
#include <kernel/drv/tty.h>
#include <string.h>
#include <stdlib.h>
#include <kernel/sys/isr.h>
#include <kernel/sys/idt.h>
#include <kernel/sys/ports.h>
#include <kernel/panic.h>

static struct idt_descriptor	_idt[MAX_INTERRUPTS];
static struct idtr				_idtr;

void idt_set_entry (uint32_t i, uint16_t flags, uint16_t sel, void (*irq)()) {
	uint32_t uiBase     = (uint32_t)&(*irq);
 
	_idt[i].offset_low	=	uiBase & 0xffff;
	_idt[i].offset_high	=	(uiBase >> 16) & 0xffff;
	_idt[i].reserved	=	0;
	_idt[i].flags		=	flags;
    _idt[i].sel		    =	sel;
}

void _isr0();
void _isr1();
void _isr2();
void _isr3();
void _isr4();
void _isr5();
void _isr6();
void _isr7();
void _isr8();
void _isr9();
void _isr10();
void _isr11();
void _isr12();
void _isr13();
void _isr14();
void _isr15();
void _isr16();
void _isr17();
void _isr18();
void _isr19();
void _isr20();
void _isr21();
void _isr22();
void _isr23();
void _isr24();
void _isr25();
void _isr26();
void _isr27();
void _isr28();
void _isr29();
void _isr30();
void _isr31();
void _isr32();

void _irq0();
void _irq1();
void _irq2(); 
void _irq3(); 
void _irq4(); 
void _irq5(); 
void _irq6(); 
void _irq7(); 
void _irq8(); 
void _irq9(); 
void _irq10(); 
void _irq11(); 
void _irq12(); 
void _irq13(); 
void _irq14(); 
void _irq15(); 

void set_isrs(uint16_t code_selector) {
    const int FLAGS = IDT_DESC_PRESENT | IDT_DESC_BIT32;
	idt_set_entry (0, FLAGS, code_selector, _isr0);
	idt_set_entry (1, FLAGS, code_selector, _isr1);
	idt_set_entry (2, FLAGS, code_selector, _isr2);
	idt_set_entry (3, FLAGS, code_selector, _isr3);
	idt_set_entry (4, FLAGS, code_selector, _isr4);
	idt_set_entry (5, FLAGS, code_selector, _isr5);
	idt_set_entry (6, FLAGS, code_selector, _isr6);
	idt_set_entry (7, FLAGS, code_selector, _isr7);
	idt_set_entry (8, FLAGS, code_selector, _isr8);
	idt_set_entry (9, FLAGS, code_selector, _isr9);
	idt_set_entry (10, FLAGS, code_selector, _isr10);
	idt_set_entry (11, FLAGS, code_selector, _isr11);
	idt_set_entry (12, FLAGS, code_selector, _isr12);
	idt_set_entry (13, FLAGS, code_selector, _isr13);
	idt_set_entry (14, FLAGS, code_selector, _isr14);
	idt_set_entry (15, FLAGS, code_selector, _isr15);
	idt_set_entry (16, FLAGS, code_selector, _isr16);
	idt_set_entry (17, FLAGS, code_selector, _isr17);
	idt_set_entry (18, FLAGS, code_selector, _isr18);
	idt_set_entry (19, FLAGS, code_selector, _isr19);
	idt_set_entry (20, FLAGS, code_selector, _isr20);
	idt_set_entry (21, FLAGS, code_selector, _isr21);
	idt_set_entry (22, FLAGS, code_selector, _isr22);
	idt_set_entry (23, FLAGS, code_selector, _isr23);
	idt_set_entry (24, FLAGS, code_selector, _isr24);
	idt_set_entry (25, FLAGS, code_selector, _isr25);
	idt_set_entry (26, FLAGS, code_selector, _isr26);
	idt_set_entry (27, FLAGS, code_selector, _isr27);
	idt_set_entry (28, FLAGS, code_selector, _isr28);
	idt_set_entry (29, FLAGS, code_selector, _isr29);
	idt_set_entry (30, FLAGS, code_selector, _isr30);
	idt_set_entry (31, FLAGS, code_selector, _isr31);

	idt_set_entry (32, FLAGS, code_selector, _irq0);
	idt_set_entry (33, FLAGS, code_selector, _irq1);
	idt_set_entry (34, FLAGS, code_selector, _irq2);
	idt_set_entry (35, FLAGS, code_selector, _irq3);
	idt_set_entry (36, FLAGS, code_selector, _irq4);
	idt_set_entry (37, FLAGS, code_selector, _irq5);
	idt_set_entry (38, FLAGS, code_selector, _irq6);
	idt_set_entry (39, FLAGS, code_selector, _irq7);
	idt_set_entry (40, FLAGS, code_selector, _irq8);
	idt_set_entry (41, FLAGS, code_selector, _irq9);
	idt_set_entry (42, FLAGS, code_selector, _irq10);
	idt_set_entry (43, FLAGS, code_selector, _irq11);
	idt_set_entry (44, FLAGS, code_selector, _irq12);
	idt_set_entry (45, FLAGS, code_selector, _irq13);
	idt_set_entry (46, FLAGS, code_selector, _irq14);
	idt_set_entry (47, FLAGS, code_selector, _irq15);
}

void set_isrs(uint16_t codeSel);

void idt_init(uint16_t code_selector) {
	_idtr.limit = sizeof (struct idt_descriptor) * MAX_INTERRUPTS-1;
	_idtr.base	= (uint32_t)&_idt[0];
	memset((void*)&_idt[0], 0, sizeof(struct idt_descriptor) * MAX_INTERRUPTS-1);

    set_isrs(code_selector);
 
	__asm__( "lidt (%0)" :: "m" (_idtr) );

    check();
    printf("IDT initialization completed successfully!\n");
}