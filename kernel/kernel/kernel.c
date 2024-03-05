#include <kernel/kernel.h>
multiboot_info_t* multiboot;



void kernel_early(__attribute__((unused)) multiboot_info_t* mbd, __attribute__((unused)) unsigned int magic) {
    terminal_initialize(); 
    init_hal(mbd);
	multiboot = mbd;
    check();
    printf("PC Speaker testing!\n\n");
    beep(6, 10);
    terminal_startscreen();
    printf("/>");


    keyboard_init();
    
}

void kernel_main(void) {
    for (;;) ;
}