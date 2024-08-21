#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <kernel/vfs.h>

static filesystem_t registered_filesystems[FILESYSTEM_MAX_COUNT] = {0};
static fs_object_t registered_mountpoints[MOUNTPOINTS_MAX_COUNT] = {0};

int find_free_fs_nr() {
    for (int i = 0; i < 32; i++) {
        if (!registered_filesystems[i].valid) {
            return i;
        }
    }

    return -1;
}

int register_filesystem(const char* name, probe_fn_t probe, diropen_fn_t diropen, fileopen_fn_t fileopen,
                fileread_fn_t fileread, filewrite_fn_t filewrite, fileclose_fn_t fileclose) {
    int fs_nr = find_free_fs_nr();

    if (fs_nr == -1) {
        return -1;
    }

    registered_filesystems[fs_nr].valid = true;
    registered_filesystems[fs_nr].name = name;
    registered_filesystems[fs_nr].probe = probe;
    registered_filesystems[fs_nr].diropen = diropen;
    registered_filesystems[fs_nr].fileopen = fileopen;
    registered_filesystems[fs_nr].fileread = fileread;
    registered_filesystems[fs_nr].filewrite = filewrite;
    registered_filesystems[fs_nr].fileclose = fileclose;

    return fs_nr;
}

int find_free_mountpoint_nr() {
    for (int i = 0; i < 32; i++) {
        if (!registered_mountpoints[i].valid) {
            return i;
        }
    }

    return -1;
}

int register_mountpoint(size_t disk_nr, filesystem_t* fs, void* priv_data) {
    int mp_nr = find_free_mountpoint_nr();

    if (mp_nr == -1) {
        return -1;
    }

    registered_mountpoints[mp_nr].valid = true;
    registered_mountpoints[mp_nr].disk_nr = disk_nr;
    registered_mountpoints[mp_nr].filesystem = fs;
    registered_mountpoints[mp_nr].priv_data = priv_data;

    return mp_nr;
}

void vfs_scan() {
    // TODO: Really scan disks on filesystems, but it's a simple test, so we just add a mountpoint here
    
    for(int disk = 0; disk < 32; disk++) {
        // Probe every filesystem
        for(int fsn = 0; fsn < 32; fsn++) {
            if(!registered_filesystems[fsn].valid) {
                continue;
            }

            bool result = registered_filesystems[fsn].probe(disk, registered_mountpoints + disk);

            if(result) {
                register_mountpoint(disk, registered_filesystems + fsn, NULL);
                printf("Filesystem %s registered on disk %d!\n", registered_filesystems[fsn].name, disk);
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

    if (disk_nr >= 32) {
        return NULL;
    }

    fs_object_t* mt;
    
    for(int i = 0; i < 32; i++) {
        if(registered_mountpoints[i].disk_nr == disk_nr) {
            mt = registered_mountpoints + i;
            break;
        }
    }

    if(!mt) {
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

    if (disk_nr >= 32) {
        return NULL;
    }

    fs_object_t* mt;
    
    for(int i = 0; i < MOUNTPOINTS_MAX_COUNT; i++) {
        if(registered_mountpoints[i].disk_nr == disk_nr) {
            mt = registered_mountpoints + i;
            break;
        }
    }

    if(!mt) {
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


