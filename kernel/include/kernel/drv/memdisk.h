#pragma once

#include <kernel/disk_manager.h>
#include <stdint.h>

struct memdisk_info_t {
    void* memory;
    unsigned long long allocated_size;
};

void memdisk_attach(disk_t disk);
void memdisk_read(disk_t disk, uint64_t offset, uint32_t size, void* out);
void memdisk_write(disk_t disk, uint64_t offset, uint32_t size, const void* out);
uint64_t memdisk_get_capacity(disk_t disk);
void memdisk_detach(disk_t disk);
void memdisk_init(uint64_t capacity);

