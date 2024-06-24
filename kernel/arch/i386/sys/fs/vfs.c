#include <string.h>
#include <kernel/kernel.h>
#include <stdint.h>


static vfs_t vfs;

void vfs_init(fs_type_t type, vfs_adapter_t *fs_adapter)
{
    vfs_register(type, fs_adapter);
}

void vfs_register(fs_type_t type, vfs_adapter_t *fs_adapter)
{
    switch (type) {
        
        case INITRD:
            printf(" kernel: VFS: set filesystem %d (INITRD)\n", type);
            break;
        
        default:
            printf("unknown file system: %d\n", type);
            panic("VFS ERORR", "vfs.c", 23);
            break;
    };

    bzero(&vfs, sizeof(vfs_t));

    vfs.open   = fs_adapter->open; 
    vfs.close  = fs_adapter->close;
    vfs.read   = fs_adapter->read;
    vfs.write  = fs_adapter->write;
    vfs.creat  = fs_adapter->creat;
    vfs.unlink = fs_adapter->unlink;
}

int vfs_open(const char *pathname, int flags)
{
    if (!vfs.open)
        return -1;
    
    return vfs.open(pathname, flags);
}

int vfs_close(int fd)
{
    if (!vfs.close)
        return -1;
    
    return vfs.close(fd);
}

int vfs_read(int fd, void *buf, size_t count)
{
    if (!vfs.read)
        return -1;
    
    return vfs.read(fd, buf, count);
}

int vfs_write(int fd, void *buf, size_t count)
{
    if (!vfs.write)
        return -1;
    
    return vfs.write(fd, buf, count);
}

int vfs_creat(const char* pathname, mode_t mode)
{
    if (!vfs.creat)
        return -1;
    
    return vfs.creat(pathname, mode);
}

int vfs_unlink(const char* pathname)
{
    if (!vfs.unlink)
        return -1;
    
    return vfs.unlink(pathname);
}