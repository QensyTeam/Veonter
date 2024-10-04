#include "kernel/drv/ps2_keyboard.h"
#include "kernel/sys/gui/psf.h"
#include <kernel/kernel.h>
#include <kernel/drv/memdisk.h>
#include <kernel/drv/ata_pio.h>
#include <kernel/drv/pci.h>
#include <kernel/drv/ps2.h>
#include <kernel/drv/ps2_mouse.h>
#include <kernel/drv/serial_port.h>
#include <kernel/time.h>
#include <kernel/drv/rtc.h>

extern rgb_color_t fg_color;
extern rgb_color_t bg_color;

#define HEAP_START_ADDRESS 0x1000000

void irq_disable();
void irq_enable();
void test_elf(uint32_t *addr, uint32_t count);

int init_hal(__attribute__((unused)) multiboot_info_t* multiboot_info) {
    vbe_mode_info_t* vbe_mode_info = (vbe_mode_info_t*) multiboot_info->vbe_mode_info;
    uint32_t framebuffer_address = vbe_mode_info->physbase;

// Рассчитываем размер кучи
    size_t heap_size = calculate_heap_size(multiboot_info);
    
    serial_port_init(COM1);

    // Инициализируем кучу
    kheap_init((void*)HEAP_START_ADDRESS, heap_size);

    set_video_mode(800, 600, 32, (uint32_t*)framebuffer_address); 
    
    psf_v1_init();

    irq_disable();
    
    initialize_screen();  // Инициализируем экранные параметры
    
    gdt_init();
    idt_init(GDT_CODE_SEL_1);
    
    pic_init();
    timer_init();
    
    set_time_provider(rtc_time_provider);

    irq_enable();

    beep(6, 100);
    check();
    printf("PC Speaker testing!\n");

    pci_init();
    pci_scan_everything();

    memdisk_init(1 << 17);
    ata_init();
    printf("\n");
    shell_text_color(RGB(150, 150, 150));
    detect_cpu();
    meminfo_program();
    test_elf(multiboot_info->mods_addr, multiboot_info->mods_count);
    shell_text_color(RGB(255, 255, 255));
    sleep(1000);
    vbe_clear_screen(bg_color);
    printf("\n");
    logo();
    printf("\n");

    ps2_init();
    ps2_keyboard_preinit();
    ps2_mouse_preinit();

    // Limit mouse coordinates to screend bounds
    ps2_mouse_set_bounds(1024, 768);

    ps2_keyboard_init();
    ps2_mouse_init();
    return 0;
}
