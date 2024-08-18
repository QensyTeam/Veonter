#include "kernel/drv/ps2_keyboard.h"
#include "kernel/sys/gui/psf.h"
#include <kernel/kernel.h>
#include <kernel/drv/memdisk.h>
#include <kernel/drv/ata_pio.h>
#include <kernel/drv/ps2.h>
#include <kernel/drv/ps2_mouse.h>
#include <kernel/drv/serial_port.h>

extern rgb_color_t fg_color;
extern rgb_color_t bg_color;

#define HEAP_START_ADDRESS 0x1000000

void irq_disable();
void irq_enable();

int init_hal(__attribute__((unused)) multiboot_info_t* multiboot_info) {
    vbe_mode_info_t* vbe_mode_info = (vbe_mode_info_t*) multiboot_info->vbe_mode_info;
    uint32_t framebuffer_address = vbe_mode_info->physbase;

// Рассчитываем размер кучи
    size_t heap_size = calculate_heap_size(multiboot_info);

    // Инициализируем кучу
    kheap_init((void*)HEAP_START_ADDRESS, heap_size);

    set_video_mode(1024, 768, 32, (uint32_t*)framebuffer_address); 
    //psf_init();
    psf_v1_init();

    irq_disable();
    initialize_screen();  // Инициализируем экранные параметры
    gdt_init();
    idt_init(GDT_CODE_SEL_1);
    pic_init();
    timer_init();
    irq_enable();
    beep(6, 100);
    check();
    printf("PC Speaker testing!\n");
    memdisk_init(1 << 17);
    ata_init();
    printf("\n");
    shell_text_color(RGB(150, 150, 150));
    detect_cpu();
    shell_text_color(RGB(255, 255, 255));
    printf("\n");
    logo();

    ps2_init();
    ps2_keyboard_preinit();
    ps2_mouse_preinit();

    // Limit mouse coordinates to screend bounds
    ps2_mouse_set_bounds(1024, 768);

    ps2_keyboard_init();
    ps2_mouse_init();

    serial_port_init(COM1);
    serial_write_string(COM1, "Pika-pika-pikachu!\n");
    qemu_log("KERNEL RUNNING");

    return 0;
}
