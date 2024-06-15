#ifndef FAT16_H
#define FAT16_H

#include <stdint.h>

#define FILENAME_LENGTH 8
#define EXTENSION_LENGTH 3
#define FAT_ENTRY_SIZE 2
#define ROOT_DIRECTORY_SIZE 512
#define CLUSTER_SIZE 4096

typedef struct {
    char filename[FILENAME_LENGTH];
    char extension[EXTENSION_LENGTH];
    uint16_t starting_cluster;
    uint32_t size;
} fat16_file_entry_t;

typedef struct {
    uint16_t entries[ROOT_DIRECTORY_SIZE];
} fat16_fat_t;

typedef struct {
    fat16_file_entry_t* root_directory;
    fat16_fat_t* fat;
    void* data_region;
} fat16_fs_t;

fat16_fs_t* fat16_init(void* disk);
void fat16_read_file(fat16_fs_t* fs, const char* filename);

#endif /* FAT16_H */
