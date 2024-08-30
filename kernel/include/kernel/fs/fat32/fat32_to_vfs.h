#pragma once

#include <kernel/vfs.h>

direntry_t* fat32_diropen(fs_object_t* obj, const char* path);
void fat32_dirclose(fs_object_t* obj, direntry_t* dir);
NFILE* fat32_fileopen(fs_object_t* obj, const char* path);
size_t fat32_fileread(fs_object_t* fs, void* data, size_t size, size_t count, NFILE* fp);
size_t fat32_filewrite(fs_object_t* fs, const void* data, size_t size, size_t count, NFILE* fp);
void fat32_fileclose(fs_object_t* fs, NFILE* fp);

int fat32_mkdir(fs_object_t* fs, const char* path);
int fat32_touch(fs_object_t* fs, const char* path);
