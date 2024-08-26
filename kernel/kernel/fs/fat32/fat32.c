#include <kernel/fs/fat32/fat32.h>
#include <kernel/fs/fat32/fat_utf16_utf8.h>
#include <kernel/fs/fat32/lfn.h>
#include <kernel/vfs.h>
#include <kernel/disk_manager.h>
#include <kernel/drv/serial_port.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fast_traverse(direntry_t* dir) {
    do {
        printf("T: %d; Name: %s; Size: %zu; (-> %p) (priv: %u)\n", dir->type, dir->name, dir->size, dir->next, dir->priv_data);
        dir = dir->next;
    } while(dir);
}

bool fat32_init(size_t disk_nr, fs_object_t* obj) {
    fat_t* fat = calloc(1, sizeof(FATInfo_t));

    FATInfo_t* info = calloc(1, sizeof(FATInfo_t));
    diskmgr_read(disk_nr, 0, sizeof(FATInfo_t), info);
    fat->fat = info;

    if(memcmp(info->fs_type, "FAT32", 5) != 0) {
        qemu_log("FAT32 is not in %d", disk_nr);
        free(fat);
        free(info);
        return false;
    }

    fat->cluster_size = info->bytes_per_sector * info->sectors_per_cluster;
    fat->fat_offset = info->reserved_sectors * info->bytes_per_sector;
    fat->fat_size = info->fat_size_in_sectors * info->bytes_per_sector;
    fat->reserved_fat_offset = (info->reserved_sectors + info->fat_size_in_sectors) * info->bytes_per_sector;

    uint32_t two_fats = info->fat_size_in_sectors * 2;
    uint32_t tot_cluster = (info->reserved_sectors + two_fats) + ((info->root_directory_offset_in_clusters - 2) * info->sectors_per_cluster);

    fat->cluster_base = ((info->reserved_sectors + two_fats) - 2) * info->sectors_per_cluster * info->bytes_per_sector;
    fat->root_directory_offset = tot_cluster * info->bytes_per_sector;

    fat->fat_chain = calloc(fat->fat_size, 1);
    //fseek(fat->image, fat->fat_offset, SEEK_SET);
    //fread(fat->fat_chain, fat->fat_size, 1, file);
    diskmgr_read(disk_nr, fat->fat_offset, fat->fat_size, fat->fat_chain);
   
    obj->disk_nr = disk_nr;
    obj->priv_data = fat;

    qemu_log("FAT: %x", fat);
    qemu_log("Info: %x", fat->fat);
    qemu_log("Root: %d", fat->fat->root_directory_offset_in_clusters);

    return true;
}

void fat32_deinit(fat_t* fat) {
    free(fat->fat_chain);
    free(fat->fat);
}

direntry_t* fat32_read_directory(fs_object_t* obj, fat_t* fat, uint32_t start_cluster) {
    uint32_t cluster_count = fat32_read_cluster_chain(obj, fat, start_cluster, true, NULL);

    char* cluster_data = calloc(cluster_count, fat->cluster_size);
    fat32_read_cluster_chain(obj, fat, start_cluster, false, cluster_data);

    int32_t current_offset = cluster_count * fat->cluster_size - 32;   // We must start from the end.

    uint32_t cluster_size = fat->cluster_size;
    uint32_t offset = start_cluster * cluster_size;

    direntry_t* dir = calloc(1, sizeof(direntry_t));
    direntry_t* dirptr = dir;

    // HINT: Both LFN and directory entry are 32 bytes long.
    DirectoryEntry_t* prev = (DirectoryEntry_t*)(cluster_data + cluster_count);
    uint16_t in_name_buffer[256] = {0};
    size_t in_name_ptr = 0;
    char out_name_buffer[256] = {0};

    do {
        DirectoryEntry_t* entry = (DirectoryEntry_t*)(cluster_data + current_offset);

        if (entry->name[0] == 0x00) {
            goto next;
        }

        if ((uint8_t)entry->name[0] == 0xE5) {
            // Удалённый файл
            goto next;
        }

        if (entry->attributes & ATTR_LONG_FILE_NAME) {
            LFN_t* lfn = (LFN_t*)(cluster_data + current_offset);

            for(int p1 = 0; p1 < 5; p1++) {
                if(lfn->first_name_chunk[p1] == 0x0000) {
                    goto lfn_next;
                }

                in_name_buffer[in_name_ptr++] = lfn->first_name_chunk[p1];
            }

            for(int p2 = 0; p2 < 6; p2++) {
                if(lfn->second_name_chunk[p2] == 0x0000) {
                    goto lfn_next;
                }

                in_name_buffer[in_name_ptr++] = lfn->second_name_chunk[p2];
            }
           
            for(int p3 = 0; p3 < 2; p3++) {
                if(lfn->third_name_chunk[p3] == 0x0000) {
                    goto lfn_next;
                }

                in_name_buffer[in_name_ptr++] = lfn->third_name_chunk[p3];
            }

lfn_next:
            if(lfn->attr_number & 0x40) {
                utf16_to_utf8(in_name_buffer, in_name_ptr, out_name_buffer);

                dirptr->name = calloc(256, 1);
                memcpy(dirptr->name, out_name_buffer, 256);

                dirptr->type = (prev->attributes & ATTR_DIRECTORY) ? ENT_DIRECTORY : ENT_FILE;
                dirptr->size = prev->file_size;
                dirptr->priv_data = (void*)(size_t)((prev->high_cluster << 16) | prev->low_cluster);

                if(current_offset != 0) {
                    dirptr->next = calloc(1, sizeof(direntry_t));
                    dirptr = dirptr->next;
                }

                memset(in_name_buffer, 0, 512);
                memset(out_name_buffer, 0, 256);
                in_name_ptr = 0;
            }

            current_offset -= 32;
            continue;
        } 
       
        if(!(current_offset - 32 >= 0 && ((entry - 1)->attributes & ATTR_LONG_FILE_NAME))) {
            dirptr->name = calloc(12, 1);
            memcpy(dirptr->name, entry->name, 8);
            memcpy(dirptr->name + 8, entry->ext, 3);

            dirptr->type = (entry->attributes & ATTR_DIRECTORY) ? ENT_DIRECTORY : ENT_FILE;
            dirptr->size = entry->file_size;
            dirptr->priv_data = (void*)(size_t)((entry->high_cluster << 16) | entry->low_cluster);

            if(current_offset != 0) {
                dirptr->next = calloc(1, sizeof(direntry_t));
                dirptr = dirptr->next;
            }
        }
next:
        current_offset -= 32;

        prev = entry;
    } while (current_offset >= 0); 

    free(cluster_data);

    return dir;
}

// Returns cluster count and reads cluster data.
size_t fat32_read_cluster_chain(fs_object_t* obj, fat_t* fat, uint32_t start_cluster, bool probe, void* out) {
    uint32_t cluster_count = 0;

    uint32_t cluster_size = fat->cluster_size;
    uint32_t cluster = start_cluster;
    while (cluster < 0x0FFFFFF8) {  // Проверка на последний кластер в цепочке
        if(!probe) {
            uint32_t offset = fat->cluster_base + (cluster * cluster_size);

            diskmgr_read(obj->disk_nr, offset, cluster_size, ((char*)out) + (cluster_count * cluster_size));
        }
        
        // Перейти к следующему кластеру
        cluster = fat->fat_chain[cluster];
        cluster_count++;
    }

    return cluster_count;
}

// Returns count of read bytes!!!
size_t fat32_read_cluster_chain_advanced(fs_object_t* obj, fat_t* fat, uint32_t start_cluster, size_t byte_offset, size_t size, bool probe, void* out) {
    uint32_t cluster_size = fat->cluster_size;
    uint32_t cluster = start_cluster;
    size_t total_bytes_read = 0;
    size_t cluster_count = 0;
    
// Skip initial clusters based on byte_offset
    while (byte_offset >= cluster_size) {
        cluster = fat->fat_chain[cluster];
        byte_offset -= cluster_size;
        cluster_count++;
        
        if (cluster >= 0x0FFFFFF8) {
            return total_bytes_read;  // End of chain reached
        }
    }

    // Now we are positioned at the correct cluster and byte offset within it
    while (cluster < 0x0FFFFFF8 && total_bytes_read < size) {
        if (!probe) {
            uint32_t offset = fat->cluster_base + (cluster * cluster_size) + byte_offset;
            size_t bytes_to_read = cluster_size - byte_offset;
            
            // Ensure we don’t read more than requested
            if (bytes_to_read > size - total_bytes_read) {
                bytes_to_read = size - total_bytes_read;
            }

            //fseek(fat->image, offset, SEEK_SET);
            //fread(((char*)out) + total_bytes_read, bytes_to_read, 1, fat->image);

            diskmgr_read(obj->disk_nr, offset, bytes_to_read, ((char*)out) + total_bytes_read);

            total_bytes_read += bytes_to_read;
        }

        // Move to the next cluster in the chain
        cluster = fat->fat_chain[cluster];
        cluster_count++;
        byte_offset = 0;  // Only the first iteration needs a non-zero byte offset
    }

    return total_bytes_read;
}


size_t fat32_search_on_cluster(fs_object_t* obj, fat_t* fat, size_t cluster, const char* name) {
    uint32_t found_cluster = 0;

    direntry_t* entries = fat32_read_directory(obj, fat, cluster);
    direntry_t* orig = entries;

    //fast_traverse(entries);

    do {
        if(strcmp(entries->name, name) == 0) {
            found_cluster = (uint32_t)(size_t)entries->priv_data;
            break;
        }

        entries = entries->next;
    } while(entries);

    // FIXME: KLUDGE! Replace it with `dirclose` when port to Veonter (Other OS)
    do {
        direntry_t* k = orig;
        orig = orig->next;
        free(k);
    } while(orig);

    return found_cluster;
}

size_t fat32_search(fs_object_t* obj, fat_t* fat, const char* path) {
    // Start at the root directory
    size_t cluster = fat->fat->root_directory_offset_in_clusters;

    qemu_log("Obj: %x; FAT: %x; Info: %x\n", obj, fat, fat->fat);

    qemu_log("%d / %x", cluster, cluster);
    // Temporary buffer for directory or file names
    char temp_name[256] = {0};

    while (*path != '\0') {
        // Skip leading slashes (handles cases like ///a///b/c.txt)
        while (*path == '/') {
            path++;
        }

        if (*path == '\0') {
            break; // Reached end of path
        }

        // Find the end of the current directory or file name
        const char* next_slash = path;
        while (*next_slash != '/' && *next_slash != '\0') {
            next_slash++;
        }

        // Calculate length of the name
        size_t name_length = next_slash - path;
        if (name_length >= sizeof(temp_name)) {
            // Name too long to fit in our buffer
            return 0;
        }

        // Copy the name into temp_name
        strncpy(temp_name, path, name_length);
        temp_name[name_length] = '\0';

        // Search for the current segment in the current cluster
        cluster = fat32_search_on_cluster(obj, fat, cluster, temp_name);
        if (cluster == 0) {
            // If not found, return 0
            return 0;
        }

        // Move path pointer to the next segment
        path = next_slash;
    }

    return cluster;
}

size_t fat32_get_file_size(fs_object_t* obj, fat_t* fat, const char* filename) {
    size_t len = strlen(filename);
    const char* end = filename + len;

    while(*end != '/') {
        end--;
    }

    char path[256] = {0};
    char e_filename[256] = {0};

    strncpy(path, filename, end - filename);
    strncpy(e_filename, end + 1, len - (end - filename));
   

    size_t clust = fat32_search(obj, fat, path);

    if(clust == 0) {
        return clust;
    }

    direntry_t* entries = fat32_read_directory(obj, fat, clust);
    direntry_t* orig = entries;

    size_t sz = 0;

    do {
        if(strcmp(entries->name, e_filename) == 0) {
            sz = entries->size;
            break;
        }
        
        entries = entries->next;
    } while(entries);

    dirclose(orig);

    return sz;
}

size_t fat32_find_free_cluster(fat_t* fat) {
    for(int i = 0, sz = fat->fat_size / sizeof(uint32_t); i < sz; i++) {
        if(fat->fat_chain[i] == 0) {
            return i;
        }
    }

    return 0;
}

void fat32_find_free_entry(fs_object_t* obj, fat_t* fat, size_t dir_cluster, size_t* out_cluster_number, size_t* out_offset) {
    size_t cluster_count = fat32_read_cluster_chain(obj, fat, dir_cluster, true, NULL);

    char* cluster_data = calloc(cluster_count, fat->cluster_size);
    fat32_read_cluster_chain(obj, fat, dir_cluster, false, cluster_data);

    size_t entry_size = 32;  // Directory entry size is 32 bytes
    size_t total_entries = (cluster_count * fat->cluster_size) / entry_size;

    for (size_t i = 0; i < total_entries; i++) {
        char* entry = cluster_data + (i * entry_size);

        if (entry[0] == 0x00 || (uint8_t)entry[0] == 0xE5) {
            //size_t cluster_index = i / (fat->cluster_size / entry_size);
            size_t offset_within_cluster = (i % (fat->cluster_size / entry_size)) * entry_size;

            *out_cluster_number = dir_cluster;
            *out_offset = offset_within_cluster;

            free(cluster_data);
            return;
        }
    }

    *out_cluster_number = 0;
    *out_offset = 0;
    free(cluster_data);
}

size_t fat32_get_last_cluster_in_chain(fat_t* fat, size_t start_cluster) {
    if (start_cluster < 2 || start_cluster >= 0x0FFFFFF8) {
        return 0;  // Invalid start cluster
    }

    size_t current_cluster = start_cluster;

    while (current_cluster < 0x0FFFFFF8) {
        size_t next_cluster = fat->fat_chain[current_cluster];

        if (next_cluster >= 0x0FFFFFF8) {
            break;  // Reached the end of the chain
        }

        current_cluster = next_cluster;
    }

    return current_cluster;
}
