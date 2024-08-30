#include "kernel/drv/speaker.h"
#include "kernel/sys/console.h"
#include <kernel/vfs.h>
#include <kernel/fs/fat32/fat32.h>
#include <kernel/fs/fat32/fat32_to_vfs.h>
#include <kernel/kernel.h>

multiboot_info_t* multiboot = 0;

void kernel_early(__attribute__((unused)) multiboot_info_t* mbd, __attribute__((unused)) unsigned int magic) {
    init_hal(mbd);

    // Initialize kernel-independent modules here.
    
    register_filesystem("FAT32", 
            fat32_init,
            fat32_diropen,
            fat32_dirclose,
            fat32_fileopen,
            fat32_fileread,
            fat32_filewrite,
            fat32_fileclose,
            fat32_mkdir,
            fat32_touch
    );

    vfs_scan();

    multiboot = mbd;
}

void kernel_main(void) {
    console_input_loop(); // Запуск терминала
    for (;;) ;
}

