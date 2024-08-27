#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <kernel/vfs.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN 0x02
#define ATTR_SYSTEM 0x04
#define ATTR_VOLUME_ID 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE 0x20

#define ATTR_LONG_FILE_NAME 0x0F
#define ATTR_LFN_MASK 0x3F

typedef struct {
    char bootcode[3];
    char OEM[8];
    u16  bytes_per_sector;
    u8   sectors_per_cluster;
    u16  reserved_sectors;
    u8   copies;
    u16  root_entries;
    u16  small_sectors_number;
    u8   descriptor;
    u16  sectors_per_fat;
    u16  sectors_per_track;
    u16  heads;
    u32  hidden_sectors;
    u32  sectors_in_partition;
    u32  fat_size_in_sectors;
    u16  flags;
    u16  version_num;
    u32  root_directory_offset_in_clusters;
    u16  fsinfo_sector;
    u16  _;
    char reserved1[12];
    u8   disk_number;
    u8   flags1;
    u8   extended_boot_signature;
    u32  volume_serial_number;
    char volume_label[11];
    char fs_type[8];
    char bootcode_next[];
} __attribute__((packed)) FATInfo_t;

typedef struct {
    //FILE* image;
    FATInfo_t* fat;

    uint32_t* fat_chain;

    uint32_t cluster_size;
    uint32_t fat_offset;
    uint32_t fat_size;
    uint32_t reserved_fat_offset;
    uint32_t root_directory_offset;
    uint32_t cluster_base;
} fat_t;

typedef struct {
    char name[8];
    char ext[3];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t creation_time_tenths;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t high_cluster;
    uint16_t modification_time;
    uint16_t modification_date;
    uint16_t low_cluster;
    uint32_t file_size;
} __attribute__((packed)) DirectoryEntry_t;

bool fat32_init(size_t disk_nr, fs_object_t* obj);
void fat32_deinit(fat_t* fat);
direntry_t* fat32_read_directory(fs_object_t* obj, fat_t* fat, uint32_t start_cluster);
void read_file_data(fs_object_t* obj, fat_t* fat, uint32_t start_cluster);
size_t fat32_read_cluster_chain(fs_object_t* obj, fat_t* fat, uint32_t start_cluster, bool probe, void* out);
size_t fat32_search_on_cluster(fs_object_t* obj, fat_t* fat, size_t cluster, const char* name);
size_t fat32_search(fs_object_t* obj, fat_t* fat, const char* path);
size_t fat32_get_file_size(fs_object_t* obj, fat_t* fat, const char* filename);
size_t fat32_read_cluster_chain_advanced(fs_object_t* obj, fat_t* fat, uint32_t start_cluster, size_t byte_offset, size_t size, bool probe, void* out);
size_t fat32_write(fs_object_t* obj, fat_t* fat, const char* path, size_t offset, size_t size, const char* buffer);

