#include "kernel/drv/speaker.h"
#include "kernel/sys/console.h"
#include <kernel/vfs.h>
#include <kernel/kernel.h>

multiboot_info_t* multiboot = 0;

void kernel_early(__attribute__((unused)) multiboot_info_t* mbd, __attribute__((unused)) unsigned int magic) {
    init_hal(mbd);

    // Initialize kernel-independent modules here.
    
    vfs_scan();

    multiboot = mbd;

    printf(PROMPT_STRING);
}

void kernel_main(void) {
    console_input_loop(); // Запуск терминала
    for (;;) ;
}

