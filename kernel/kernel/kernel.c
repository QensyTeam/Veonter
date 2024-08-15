#include "kernel/drv/speaker.h"
#include "kernel/sys/console.h"
#include "kernel/drv/memdisk.h"
#include <kernel/kernel.h>

multiboot_info_t* multiboot = 0;

int LANGUAGE = 2;   // English by fallback.
                    //
                    // 1. Russian
                    // 2. English

void kernel_early(__attribute__((unused)) multiboot_info_t* mbd, __attribute__((unused)) unsigned int magic) {
    init_hal(mbd);

    memdisk_init(1 << 20);

    multiboot = mbd;


    int choice = -1;

    while(choice != 1 && choice != 2) {
        printf("Select language/Выберите язык: \n1. Русский\n2. English\n> ");
        scanf("%d", &choice);
    }

    LANGUAGE = choice;

    printf(PROMPT_STRING);
}

void kernel_main(void) {
    console_input_loop(); // Запуск терминала
    for (;;) ;
}

