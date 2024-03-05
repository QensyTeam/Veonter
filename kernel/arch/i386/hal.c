#include <kernel/kernel.h>

void irq_disable();
void irq_enable();

int init_hal(__attribute__((unused)) multiboot_info_t* multiboot_info) {
    irq_disable();
    gdt_init();
    idt_init(GDT_CODE_SEL_1);
    pic_init();
    paging_initialize();
    timer_init();



    irq_enable();
    return 0;
}
