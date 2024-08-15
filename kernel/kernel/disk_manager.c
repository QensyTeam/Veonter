#include <disk_manager.h>
#include <stdint.h>
#include <string.h>

disk_t disks[DISK_COUNT] = {0};

int diskmgr_get_free_slot() {
    for(int i = 0; i < DISK_COUNT; i++) {
        if(!disks[i].valid) {
            return i;
        }
    }

    return -1;
}

int diskmgr_add_disk(void* priv_data, disk_attach_fn_t attach, disk_read_fn_t rdfunc, disk_write_fn_t wrfunc, disk_get_capacity_fn_t get_capacity, disk_detach_fn_t detach) {
    int slot = diskmgr_get_free_slot();

    if(slot == -1) {
        return -1;
    }

    disks[slot].valid = true;

    disks[slot].priv_data = priv_data;
    
    disks[slot].attach = attach;
    disks[slot].read = rdfunc;
    disks[slot].write = wrfunc;
    disks[slot].get_capacity = get_capacity;
    disks[slot].detach = detach;

    attach(disks[slot]);

    return slot;
}

void diskmgr_read(int disk_nr, uint64_t offset, uint32_t size, void* out_buffer) {
    if(disk_nr < 0 || disk_nr >= DISK_COUNT) {
        return;
    }

    disk_t disk = disks[disk_nr];
    
    disk.read(disk, offset, size, out_buffer);
}

void diskmgr_write(int disk_nr, uint64_t offset, uint32_t size, const void* in_buffer) {
    if(disk_nr < 0 || disk_nr >= DISK_COUNT) {
        return;
    }

    disk_t disk = disks[disk_nr];
    
    disk.write(disk, offset, size, in_buffer);
}

void diskmgr_get_capacity(int disk_nr) {
    if(disk_nr < 0 || disk_nr >= DISK_COUNT) {
        return;
    }

    disk_t disk = disks[disk_nr];
    
    disk.get_capacity(disk);
}

void diskmgr_detach(int disk_nr) {
    if(disk_nr < 0 || disk_nr >= DISK_COUNT) {
        return;
    }

    disk_t disk = disks[disk_nr];
    
    disk.detach(disk);

    memset(disks + disk_nr, 0, sizeof(disk_t));
}
