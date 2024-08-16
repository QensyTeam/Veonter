#include <kernel/kernel.h>
#include <disk_manager.h>

extern disk_t disks[DISK_COUNT];

void dhv_program() {
    for(int i = 0; i < DISK_COUNT; i++) {
        if(disks[i].valid) {
            unsigned long long cap = diskmgr_get_capacity(i);
            
            printf("Disk number %d: %llu bytes (%llu KB, %llu MB)\n", i, cap, cap >> 10, cap >> 20);
        }
    }

    uint32_t disk = 0xff;

    printf("Select disk: ");

    scanf("%d", &disk);

    if(disk == 0xff) {
        printf("Invalid input.\n");
        return;
    }

    if(disk >= DISK_COUNT) {
        printf("Out of range.\n");
        return;
    }

    if(!disks[disk].valid) {
        printf("No disk\n");
        return;
    }

    char* buffer = calloc(1024, 1);

    diskmgr_read(disk, 0, 1024, buffer);

    const char* alphabet = "0123456789ABCDEF";

    for(int i = 0; i < 1024; i++) {
        if(i % 24 == 0) {
            printf("\n");
        }

        uint8_t byte = buffer[i]; 

        printf("%c%c ", alphabet[(byte >> 4) & 0xf], alphabet[byte & 0xf]);
    }

    printf("\n");

    kfree(buffer);
}
