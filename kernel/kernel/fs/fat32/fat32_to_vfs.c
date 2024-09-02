// FAT32 v1.0 driver originally written by /> NDRAEY (c) 2024
// Modifications and some fixes by Qensy (c) 2024
//
// Built for Veonter

#include "kernel/vfs.h"
#include <kernel/fs/fat32/fat32_to_vfs.h>
#include <kernel/fs/fat32/fat32.h>
#include <kernel/drv/serial_port.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t fat32_filewrite(fs_object_t *fs, const void *data, size_t size, size_t count, NFILE *fp) {
    qemu_log("TODO: FAT32 WRITE IS NOT TESTED!\n");
    
    fat_t* fat = fs->priv_data;
    
    fat32_write(fs, fat, fp->path, fp->position, size * count, data);

    return 0;
}

direntry_t* fat32_diropen(fs_object_t *obj, const char *path) {
    fat_t* fat = obj->priv_data;

    size_t cluster = fat32_search(obj, fat, path);

    if(cluster == 0) {
        return NULL;
    }

    return fat32_read_directory(obj, fat, cluster);
}

void fat32_dirclose(fs_object_t *obj, direntry_t *dir) {
    qemu_log("FAT32 directory is closed");

    (void)obj;
    (void)dir;
}

NFILE* fat32_fileopen(fs_object_t *fs, const char* path) {
    fat_t* fat = fs->priv_data;

    size_t cluster = fat32_search(fs, fat, path);

    if(cluster == 0) {
        return NULL;
    }

    NFILE* file = calloc(1, sizeof(NFILE));

    file->path = path;
    file->size = fat32_get_file_size(fs, fat, path);
    file->priv_data = (void*)cluster;   // HINT: There's no pointer function, do not dereference it!!! It's only the data we want to store there.

    return file;
}

size_t fat32_fileread(fs_object_t *fs, void *data, size_t size, size_t count, NFILE *fp) {
    fat_t* fat = fs->priv_data;
    size_t clust = (size_t)fp->priv_data;

    return fat32_read_cluster_chain_advanced(fs, fat, clust, fp->position, size * count, false, data);
}

void fat32_fileclose(fs_object_t *fs, NFILE *fp) {
    // Nothing

    (void)fs;
    (void)fp;
}

int fat32_mkdir(fs_object_t* fs, const char* path) {
    fat_t* fat = fs->priv_data;
    
    char* fpath = calloc(strlen(path) + 1, 1);
    strcpy(fpath, path);
    
    char* end = fpath + strlen(fpath);

    while(*end != '/') {
        end--;
    }

    fpath[end - fpath] = 0; // Divide path by inserting zero where path ends and where dirname begins.
    
    end++;

    qemu_log("%s", end);


    int result = -1;
    size_t entry = fat32_search(fs, fat, fpath);

    if(entry != 0) {
        qemu_log("Entry found");
    } else {
        qemu_log("Entry WAS NOT found");
        goto end;
    }

    fat32_create_object(fs, fat, entry, end, false);

    result = 0;
end:
    free(fpath);
    return result;
}

int fat32_touch(fs_object_t* fs, const char* path) {
    return 0;
}
