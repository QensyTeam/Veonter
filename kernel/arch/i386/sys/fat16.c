#include <kernel/sys/fat16.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // для функции malloc

fat16_fs_t* fat16_init(void* disk) {
    fat16_fs_t* fs = (fat16_fs_t*) malloc(sizeof(fat16_fs_t));

    fs->root_directory = (fat16_file_entry_t*) disk;
    fs->fat = (fat16_fat_t*) (disk + sizeof(fat16_file_entry_t) * ROOT_DIRECTORY_SIZE);
    fs->data_region = disk + sizeof(fat16_file_entry_t) * ROOT_DIRECTORY_SIZE + sizeof(fat16_fat_t);

    return fs;
}

void fat16_read_file(fat16_fs_t* fs, const char* filename) {
    // Найдем запись файла в корневом каталоге
    fat16_file_entry_t* entry = NULL;
    for (int i = 0; i < ROOT_DIRECTORY_SIZE; i++) {
        if (strcmp(fs->root_directory[i].filename, filename) == 0) {
            entry = &(fs->root_directory[i]);
            break;
        }
    }

    if (entry == NULL) {
        printf("File not found.\n");
        return;
    }

    // Чтение данных файла из области данных
    uint16_t current_cluster = entry->starting_cluster;
    while (current_cluster != 0xFFFF) {
        // Читаем данные из текущего кластера
        void* data = fs->data_region + (current_cluster - 2) * CLUSTER_SIZE;
        // Отобразим данные, скажем, на консоль
        printf("%s", (char*)data);

        // Перейдем к следующему кластеру
        current_cluster = fs->fat->entries[current_cluster];
    }
}
