#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <kernel/vfs.h>
#include <kernel/disk_manager.h>
#include <kernel/drv/serial_port.h>  // Debug

static filesystem_t registered_filesystems[FILESYSTEM_MAX_COUNT] = {0};
static fs_object_t registered_mountpoints[MOUNTPOINTS_MAX_COUNT] = {0};

int find_free_fs_nr() {
    for (int i = 0; i < FILESYSTEM_MAX_COUNT; i++) {
        if (!registered_filesystems[i].valid) {
            return i;
        }
    }

    return -1;
}

int register_filesystem(const char* name, probe_fn_t probe, diropen_fn_t diropen, dirclose_fn_t dirclose,
                fileopen_fn_t fileopen,
                fileread_fn_t fileread, filewrite_fn_t filewrite, fileclose_fn_t fileclose,
                mkdir_fn_t mkdir, touch_fn_t touch,
                remove_fn_t remove) {
    int fs_nr = find_free_fs_nr();

    if (fs_nr == -1) {
        return -1;
    }

    registered_filesystems[fs_nr].valid = true;
    registered_filesystems[fs_nr].name = name;
    registered_filesystems[fs_nr].probe = probe;
    registered_filesystems[fs_nr].diropen = diropen;
    registered_filesystems[fs_nr].dirclose = dirclose;
    registered_filesystems[fs_nr].fileopen = fileopen;
    registered_filesystems[fs_nr].fileread = fileread;
    registered_filesystems[fs_nr].filewrite = filewrite;
    registered_filesystems[fs_nr].fileclose = fileclose;
    
    registered_filesystems[fs_nr].mkdir = mkdir;
    registered_filesystems[fs_nr].touch = touch;

    registered_filesystems[fs_nr].remove = remove;

    return fs_nr;
}

int find_free_mountpoint_nr() {
    for (int i = 0; i < MOUNTPOINTS_MAX_COUNT; i++) {
        if (!registered_mountpoints[i].valid) {
            return i;
        }
    }

    return -1;
}

int register_mountpoint(size_t disk_nr, filesystem_t* fs) {
    int mp_nr = find_free_mountpoint_nr();

    if (mp_nr == -1) {
        return -1;
    }

    registered_mountpoints[mp_nr].valid = true;
    registered_mountpoints[mp_nr].disk_nr = disk_nr;
    registered_mountpoints[mp_nr].filesystem = fs;
    // Priv data will be registered on `probe()`

    return mp_nr;
}

void vfs_scan() {
    // TODO: Really scan disks on filesystems, but it's a simple test, so we just add a mountpoint here
    
    for(int disk = 0; disk < DISK_COUNT; disk++) {
        // Probe every filesystem
        
        if(!disks[disk].valid) {
            continue;
        }

        for(int fsn = 0; fsn < FILESYSTEM_MAX_COUNT; fsn++) {
            if(!registered_filesystems[fsn].valid) {
                continue;
            }

            int mp_nr = find_free_mountpoint_nr();
            bool result = registered_filesystems[fsn].probe(disk, registered_mountpoints + mp_nr);

            if(result) {
                register_mountpoint(disk, registered_filesystems + fsn);
                qemu_log("Filesystem %s registered on disk %d!\n", registered_filesystems[fsn].name, disk);
            }
        }
    }
}

void vfs_parse_path(const char* full_path, size_t* out_disk_nr, char** out_path) {
    char* path = NULL;

    int disk_nr = strtol(full_path, &path, 10);

    if (path[0] == ':') {
        path++;
    } else {
        path = NULL;
    }

    *out_disk_nr = disk_nr;
    *out_path = path;
}

direntry_t* diropen(const char* path) {
    size_t disk_nr;
    char* div_path;

    vfs_parse_path(path, &disk_nr, &div_path);

    if (disk_nr >= DISK_COUNT) {
        return NULL;
    }

    fs_object_t* mt = NULL;

    for(int i = 0; i < MOUNTPOINTS_MAX_COUNT; i++) {
        if(registered_mountpoints[i].disk_nr == disk_nr && registered_mountpoints[i].valid) {
            mt = registered_mountpoints + i;
            break;
        }
    }

    if(mt == NULL) {
        return NULL;
    }

    if(!mt->valid) {
        return NULL;
    }

    return mt->filesystem->diropen(mt, div_path);
}

void dirclose(direntry_t* direntry) {
    if(!direntry) {
        return;
    }
    
    do {
        direntry_t* k = direntry;
        direntry = direntry->next;
        free(k);
    } while(direntry);
};

NFILE* nfopen(const char* path) {
    size_t disk_nr;
    char* div_path;

    vfs_parse_path(path, &disk_nr, &div_path);

    if (disk_nr >= DISK_COUNT) {
        return NULL;
    }

    fs_object_t* mt = NULL;
    
    for(int i = 0; i < MOUNTPOINTS_MAX_COUNT; i++) {
        if(registered_mountpoints[i].disk_nr == disk_nr) {
            mt = registered_mountpoints + i;
            break;
        }
    }

    if(mt == NULL) {
        return NULL;
    }

    if(!mt->valid) {
        return NULL;
    }

    NFILE* fp = mt->filesystem->fileopen(mt, div_path);
    fp->_obj = mt;

    return fp;
}

void nfclose(NFILE* file) {
    fs_object_t* mt = file->_obj;
    
    mt->filesystem->fileclose(mt, file);

    free(file);
}

size_t nfread(void* buffer, size_t size, size_t count, NFILE* file) {
    if (!file) {
        return 0;
    }

    fs_object_t* mt = file->_obj;

    size_t a = mt->filesystem->fileread(mt, buffer, size, count, file);

    /*size_t bytes_to_read = size * count;
    if (file->position + bytes_to_read > file->size) {
        bytes_to_read = file->size - file->position;
    }

    if (bytes_to_read == 0) {
        return 0;
    }*/

    return a;
}

size_t nfwrite(const void* buffer, size_t size, size_t count, NFILE* file) {
    if (!file) {
        return 0;
    }

    fs_object_t* mt = file->_obj;

    size_t a = mt->filesystem->filewrite(mt, buffer, size, count, file);
    return a;
}

int mkdir(const char* path) {
    size_t disk_nr;
    char* div_path;

    vfs_parse_path(path, &disk_nr, &div_path);

    if (disk_nr >= DISK_COUNT) {
        return -1;
    }

    fs_object_t* mt = NULL;
    
    for(int i = 0; i < MOUNTPOINTS_MAX_COUNT; i++) {
        if(registered_mountpoints[i].disk_nr == disk_nr) {
            mt = registered_mountpoints + i;
            break;
        }
    }

    if(mt == NULL) {
        return -1;
    }

    if(!mt->valid) {
        return -1;
    }

    int result = mt->filesystem->mkdir(mt, div_path);

    return result;
}

int touch(const char* path) {
    size_t disk_nr;
    char* div_path;

    vfs_parse_path(path, &disk_nr, &div_path);

    if (disk_nr >= DISK_COUNT) {
        return -1;
    }

    fs_object_t* mt = NULL;

    for(int i = 0; i < MOUNTPOINTS_MAX_COUNT; i++) {
        if(registered_mountpoints[i].disk_nr == disk_nr) {
            mt = registered_mountpoints + i;
            break;
        }
    }

    if(mt == NULL) {
        return -1;
    }

    if(!mt->valid) {
        return -1;
    }

    int result = mt->filesystem->touch(mt, div_path);

    return result;
}

int remove(const char* path) {
    qemu_log("Remove");

    size_t disk_nr;
    char* div_path;

    vfs_parse_path(path, &disk_nr, &div_path);

    if (disk_nr >= DISK_COUNT) {
        return -1;
    }

    fs_object_t* mt = NULL;

    for(int i = 0; i < MOUNTPOINTS_MAX_COUNT; i++) {
        if(registered_mountpoints[i].disk_nr == disk_nr) {
            mt = registered_mountpoints + i;
            break;
        }
    }

    if(mt == NULL) {
        return -1;
    }

    if(!mt->valid) {
        return -1;
    }

    int result = mt->filesystem->remove(mt, div_path);

    return result;
}
