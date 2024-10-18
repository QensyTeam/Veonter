#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define FILESYSTEM_MAX_COUNT 32
#define MOUNTPOINTS_MAX_COUNT 32

typedef struct datetime {
    // Date
    uint8_t day : 5;
    uint8_t month : 4;
    uint16_t year;

    // Time
    uint8_t hour : 5;
    uint8_t minute : 6;
    uint8_t second : 6;
    uint32_t millis;
} datetime_t;

typedef enum direntry_type {
    ENT_UNKNOWN = 0,
    ENT_DIRECTORY,
    ENT_FILE
} direntry_type_t;

typedef struct direntry {
    char* name;
    direntry_type_t type;

    size_t size;
    datetime_t created;
    datetime_t modified;
    
    void* priv_data;

    struct direntry* next;
} direntry_t;

struct fs_object;

typedef struct file {
    const char* path;
    size_t position;
    size_t size;

    struct fs_object* _obj;

    void* priv_data;  // FS-specific data can be stored here.
} NFILE;

struct filesystem;

typedef struct fs_object {
    bool valid;
    size_t disk_nr;  // Disk number (used by fs to read from that disk)
    struct filesystem* filesystem;

    void* priv_data;  // FS-specific data can be stored here.
} fs_object_t;

typedef bool (*probe_fn_t)(size_t, fs_object_t*);
typedef direntry_t* (*diropen_fn_t)(fs_object_t* fs, const char* path);
typedef void (*dirclose_fn_t)(fs_object_t* fs, direntry_t* entry);
typedef NFILE* (*fileopen_fn_t)(fs_object_t* fs, const char* path);
typedef size_t (*fileread_fn_t)(fs_object_t* fs, void* data, size_t size, size_t count, NFILE* fp);
typedef size_t (*filewrite_fn_t)(fs_object_t* fs, const void* data, size_t size, size_t count, NFILE* fp);
typedef void (*fileclose_fn_t)(fs_object_t* fs, NFILE* file);

typedef int (*mkdir_fn_t)(fs_object_t* fs, const char* path);
typedef int (*touch_fn_t)(fs_object_t* fs, const char* path);

typedef int (*remove_fn_t)(fs_object_t* fs, const char* path);

typedef struct filesystem {
    bool valid;
    const char* name;
    probe_fn_t probe;
    diropen_fn_t diropen;
    dirclose_fn_t dirclose;
    fileopen_fn_t fileopen;
    fileread_fn_t fileread;
    filewrite_fn_t filewrite;
    fileclose_fn_t fileclose;

    mkdir_fn_t mkdir;
    touch_fn_t touch;

    remove_fn_t remove;
} filesystem_t;

int find_free_fs_nr();
int register_filesystem(const char* name, probe_fn_t probe, diropen_fn_t diropen, dirclose_fn_t dirclose,
                        fileopen_fn_t fileopen,
                        fileread_fn_t fileread, filewrite_fn_t filewrite, fileclose_fn_t fileclose,
                        mkdir_fn_t mkdir, touch_fn_t touch,
                        remove_fn_t remove);
int find_free_mountpoint_nr();
int register_mountpoint(size_t disk_nr, filesystem_t* fs);
void vfs_scan();
void vfs_parse_path(const char* full_path, size_t* out_disk_nr, char** out_path);
direntry_t* diropen(const char* path);
void dirclose(direntry_t* direntry);
NFILE* nfopen(const char* path);
void nfclose(NFILE* file);
size_t nfread(void* buffer, size_t size, size_t count, NFILE* file);
size_t nfwrite(const void* buffer, size_t size, size_t count, NFILE* file);

int mkdir(const char* path);
int touch(const char* path);

int remove(const char* path);