#include "kernel/sys/console.h"
#include "veo.h"
#include <kernel/vfs.h>
#include <kernel/fs/fat32/fat32.h>
#include <kernel/fs/fat32/fat32_to_vfs.h>
#include <kernel/kernel.h>
#include <math.h>
#include <kernel/drv/pci.h>
#include <kernel/drv/serial_port.h>

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
            fat32_touch,
            fat32_remove
    );

    // 8086:7010
    
    // finite_pointer_t devs = pci_find_devices_by_ven_dev(0x8086, 0x7010);

    // qemu_log("Found %d devices", devs.size);

    // for(size_t i = 0; i < devs.size; i++) {
    //     pci_device_t* dev = ((pci_device_t*)devs.data) + i;

    //     qemu_log("Bus %d. Slot %d. Function %d", dev->bus, dev->slot, dev->func);
    // }

    // free(devs.data);

    vfs_scan();

//    qemu_log("Remove result: %d", remove("1:/a"));
//    qemu_log("Remove result: %d", remove("1:/b"));
//    qemu_log("Remove result: %d", remove("1:/c"));
//    qemu_log("Remove result: %d", remove("1:/123"));
//    qemu_log("Remove result: %d", remove("1:/LongfileHelloWorld"));

    multiboot = mbd;
}

void kernel_main(void) {
    console_input_loop(); // Запуск терминала
    for (;;) ;
}

