#include <stdio.h>
#include <disk_manager.h>

extern disk_t disks[128];

void disk_list() {
    for(int i = 0; i < DISK_COUNT; i++) {
        if(disks[i].valid) {
            unsigned long long cap = diskmgr_get_capacity(i);
            
            printf("Disk number %d: %llu bytes (%llu KB, %llu MB)\n", i, cap, cap >> 10, cap >> 20);
        }
    }
}
