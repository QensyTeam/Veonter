#include <kernel/kernel.h>

#define HEAP_START_ADDRESS 0x70000

void irq_disable();
void irq_enable();

int init_hal(__attribute__((unused)) multiboot_info_t* multiboot_info) {
    vbe_mode_info_t* vbe_mode_info = (vbe_mode_info_t*) multiboot_info->vbe_mode_info;
    uint32_t framebuffer_address = vbe_mode_info->physbase;

    set_video_mode(1024, 768, 32, (uint32_t*)framebuffer_address); // Set your desired resolution and color depth
    initialize_screen();  // Инициализируем экранные параметры
    psf_init();

    irq_disable();
    gdt_init();
    idt_init(GDT_CODE_SEL_1);
    pic_init();
    timer_init();
    // Рассчитываем размер кучи
    size_t heap_size = calculate_heap_size(multiboot_info);

    // Инициализируем кучу
    kheap_init((void*)HEAP_START_ADDRESS, heap_size);

    irq_enable();

    check();
    printf("PC Speaker testing!\n\n");
    beep(6, 10);
    logo();
    printf("\n");
    shell_text_color(RGB(150, 150, 150));
    detect_cpu();
    shell_text_color(fg_color);
    printf("\n");
    printf("csl> ");
    keyboard_init();
 
    return 0;
}
