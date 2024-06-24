#include <kernel/kernel.h>
multiboot_info_t* multiboot;

void kernel_early(__attribute__((unused)) multiboot_info_t* mbd, __attribute__((unused)) unsigned int magic) {
    //set_video_mode(800, 600, 32);
    
    
    //terminal_initialize(); 
    init_hal(mbd);
    multiboot = mbd;
    check();
    printf("PC Speaker testing!\n\n");
    beep(6, 10);
    logo();
    printf("csl> ");
    keyboard_init();
}

void kernel_main(void) {
    console_input_loop(); // Запуск терминала
    for (;;) ;
}
