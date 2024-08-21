#pragma once

#include <stdint.h>
#include <stdbool.h>

#define DISK_COUNT 128

struct disk;

typedef void (*disk_attach_fn_t)(struct disk);
typedef void (*disk_read_fn_t)(struct disk, uint64_t, uint32_t, void*);
typedef void (*disk_write_fn_t)(struct disk, uint64_t, uint32_t, const void*);
typedef uint64_t (*disk_get_capacity_fn_t)(struct disk);
typedef void (*disk_detach_fn_t)(struct disk);

struct disk {
    bool valid;

    disk_attach_fn_t attach;
    disk_read_fn_t read;
    disk_write_fn_t write;
    disk_get_capacity_fn_t get_capacity;
    disk_detach_fn_t detach;

    void* priv_data;
};

typedef struct disk disk_t;


// Functions

int diskmgr_get_free_slot();
int diskmgr_add_disk(void* priv_data, disk_attach_fn_t attach, disk_read_fn_t rdfunc, disk_write_fn_t wrfunc, disk_get_capacity_fn_t get_capacity, disk_detach_fn_t detach);
void diskmgr_read(int disk_nr, uint64_t offset, uint32_t size, void* out_buffer);
void diskmgr_write(int disk_nr, uint64_t offset, uint32_t size, const void* in_buffer);
uint64_t diskmgr_get_capacity(int disk_nr);
void diskmgr_detach(int disk_nr);
