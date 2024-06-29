#ifndef VFS_H_
#define VFS_H_

#include <kernel/kernel.h>

#define MAX_PATH_SIZE 64

typedef enum {
    INITRD
} fs_type_t;

typedef int mode_t; 

struct vfs_s {
    int (*open)(const char *pathname, int flags);
    int (*close)(int fd);
    int (*read)(int fd, void *buf, size_t count);
    int (*write)(int fd, void *buf, size_t count);
    int (*creat)(const char* pathname, mode_t mode);
    int (*unlink)(const char* pathname);
};

typedef struct vfs_s vfs_t;

struct vfs_adapter_s {
    int (*open)(const char *pathname, int flags);
    int (*close)(int fd);
    int (*read)(int fd, void *buf, size_t count);
    int (*write)(int fd, void *buf, size_t count);
    int (*creat)(const char* pathname, mode_t mode);
    int (*unlink)(const char* pathname);
};

typedef struct vfs_adapter_s vfs_adapter_t;

struct file_s {
    char name[MAX_PATH_SIZE]; 
    char *data; 
    u32int  size;  
};

typedef struct file_s file_t;

void vfs_register(fs_type_t type, vfs_adapter_t *fs_adapter);

void vfs_init(fs_type_t type, vfs_adapter_t *fs_adapter);

int vfs_open(const char *pathname, int flags);

int vfs_close(int fd);

int vfs_read(int fd, void *buf, size_t count);

int vfs_write(int fd, void *buf, size_t count);

int vfs_creat(const char* pathname, mode_t mode);

int vfs_unlink(const char* pathname);

#endif //VFS_H