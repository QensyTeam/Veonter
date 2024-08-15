#include <kernel/drv/memdisk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel/sys/kheap.h>

void memdisk_attach(disk_t disk) {
    struct memdisk_info_t* info = disk.priv_data;
    
    check();
    printf("Attached new Memory Disk: %llu bytes\n", info->allocated_size);
}

void memdisk_read(disk_t disk, uint64_t offset, uint32_t size, void* out) {
    struct memdisk_info_t* info = disk.priv_data;
   
    memcpy(out, ((char*)info->memory) + offset, size);
}

void memdisk_write(disk_t disk, uint64_t offset, uint32_t size, const void* out) {
    struct memdisk_info_t* info = disk.priv_data;

    memcpy(((char*)info->memory) + offset, out, size);
}

uint64_t memdisk_get_capacity(disk_t disk) {
    return ((struct memdisk_info_t*)disk.priv_data)->allocated_size;
}

void memdisk_detach(disk_t disk) {
    struct memdisk_info_t* info = disk.priv_data;

    kfree(info->memory);
    kfree(info);
}

void memdisk_init(uint64_t capacity) {
    struct memdisk_info_t* info = calloc(1, sizeof(struct memdisk_info_t));

    info->memory = calloc(capacity, 1);
    info->allocated_size = capacity;

    diskmgr_add_disk(info, memdisk_attach, memdisk_read, memdisk_write, memdisk_get_capacity, memdisk_detach);
}
