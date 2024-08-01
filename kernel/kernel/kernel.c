#include <kernel/kernel.h>
multiboot_info_t* multiboot;

void kernel_early(__attribute__((unused)) multiboot_info_t* mbd, __attribute__((unused)) unsigned int magic) {
    init_hal(mbd);
    multiboot = mbd;
}

void kernel_main(void) {
    console_input_loop(); // Запуск терминала
    for (;;) ;
}

