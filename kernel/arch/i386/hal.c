#include <kernel/kernel.h>

#define HEAP_START_ADDRESS 0x70000
#define YOUR_HEAP_SIZE 0x100000 

void irq_disable();
void irq_enable();

int init_hal(__attribute__((unused)) multiboot_info_t* multiboot_info) {
    irq_disable();
    gdt_init();
    idt_init(GDT_CODE_SEL_1);
    pic_init();
    timer_init();
    kheap_init((void*)HEAP_START_ADDRESS, sizeof(100000));


    irq_enable();
    return 0;
}
