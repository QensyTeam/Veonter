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
    qemu_log("FAT size: %d bytes", fat->fat_size);

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
            printf("Ei LFN! Set priv!");

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

size_t fat32_read_cluster_numbers(fat_t* fat, uint32_t start_cluster, uint32_t* out) {
    uint32_t cluster_count = 0;

    uint32_t cluster = start_cluster;
    
    while (cluster < 0x0FFFFFF8) {  // Проверка на последний кластер в цепочке
        out[cluster_count] = cluster;
        // Перейти к следующему кластеру
        cluster = fat->fat_chain[cluster];
        cluster_count++;
    }

    out[cluster_count] = cluster_count;

    return cluster_count;
}

void fat32_read_one_cluster(fs_object_t* obj, fat_t* fat, uint32_t cluster, void* out) {
    uint32_t cluster_size = fat->cluster_size;
    if (cluster < 0x0FFFFFF8) {  // Проверка на последний кластер в цепочке
        uint32_t offset = fat->cluster_base + (cluster * cluster_size);

        diskmgr_read(obj->disk_nr, offset, cluster_size, (char*)out);
    }
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

    do {
        qemu_log("`%s` vs our `%s` --> %d", entries->name, name, strcmp(entries->name, name));
        if(strcmp(entries->name, name) == 0) {
            found_cluster = (uint32_t)(size_t)entries->priv_data;

            printf("Found! (%d)\n", found_cluster);
            break;
        }

        entries = entries->next;
    } while(entries);

    qemu_log("Cluster: %d", found_cluster);

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

    qemu_log("Obj: %x; FAT: %x; Info: %x", obj, fat, fat->fat);

    qemu_log("Path: `%s`", path);

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

        qemu_log("Searching `%s` in cluster `%d`", temp_name, cluster);
        // Search for the current segment in the current cluster
        cluster = fat32_search_on_cluster(obj, fat, cluster, temp_name);
        if (cluster == 0) {
            qemu_log("Not found");
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

void fat32_flush(fs_object_t* obj, fat_t* fat) {
    diskmgr_write(obj->disk_nr, fat->fat_offset, fat->fat_size, fat->fat_chain);
}

size_t fat32_create_file(fs_object_t* obj, fat_t* fat, size_t dir_cluster, const char* filename, bool is_file) {
    if (filename == NULL || strlen(filename) == 0 || strlen(filename) > 255) {
        return 0;
    }

    size_t out_cluster_number = 0;
    size_t out_offset = 0;    // PIKA PIKA 
    fat32_find_free_entry(obj, fat, dir_cluster, &out_cluster_number, &out_offset);

    if (out_cluster_number == 0) {
        return 0;
    }

    size_t new_cluster = fat32_find_free_cluster(fat);
    if (new_cluster == 0) {
        return 0;
    }

    fat->fat_chain[new_cluster] = 0x0FFFFFF8;

    char sfn[12] = {0};  // 8.3 format (8 chars + '.' + 3 chars)
    LFN2SFN(filename, sfn);

    printf("SFN: %.11s\n", sfn);

    DirectoryEntry_t entry = {0};
    memset(&entry, 0, sizeof(DirectoryEntry_t));
    memcpy(entry.name, sfn, 8);
    memcpy(entry.ext, sfn + 8, 3);
    entry.attributes = is_file ? 0x20 : 0x10; // 0x20 for files, 0x10 for directories
    entry.high_cluster = (new_cluster >> 16) & 0xFFFF;
    entry.low_cluster = new_cluster & 0xFFFF;
    entry.file_size = 0; // Directories have size 0

    if(!is_file) {
        DirectoryEntry_t entry = {0};
        memset(entry.name, ' ', 8);
        memset(entry.ext, ' ', 3);

        entry.attributes |= ATTR_DIRECTORY;

        entry.name[0] = '.';

        size_t off = fat->cluster_base + (new_cluster * fat->cluster_size);

        //fseek(fat->image, off, SEEK_SET);
        //fwrite(&entry, sizeof(DirectoryEntry_t), 1, fat->image);
        diskmgr_write(obj->disk_nr, off, sizeof(DirectoryEntry_t), &entry);

        entry.name[1] = '.';
        //fwrite(&entry, sizeof(DirectoryEntry_t), 1, fat->image);
        diskmgr_write(obj->disk_nr, off + 32, sizeof(DirectoryEntry_t), &entry);
    }

    uint32_t cluster_size = fat->cluster_size;
    uint32_t entry_offset = fat->cluster_base + (out_cluster_number * cluster_size) + out_offset;
    
    unsigned short utf16_name[256] = {0};
    utf8_to_utf16(filename, utf16_name);
    
    size_t lfn_entry_count = (strlen(filename) + 12) / 13;
    
    entry_offset += lfn_entry_count * 32;

    for (size_t i = 0; i < lfn_entry_count; i++) {
        LFN_t lfn_entry = {0};
        lfn_entry.attribute = ATTR_LONG_FILE_NAME;
        lfn_entry.checksum = lfn_checksum(sfn);

        qemu_log("LFN!");

        lfn_entry.attr_number = (i == lfn_entry_count - 1) ? 0x40 : 0x00; // Set LAST_LONG_ENTRY flag for the last entry
        lfn_entry.attr_number |= (uint8_t)(i + 1); // Set the sequence number

        size_t char_index = i * 13;
        for (int p1 = 0; p1 < 5 && char_index < strlen(filename); p1++) {
            lfn_entry.first_name_chunk[p1] = utf16_name[char_index++];
        }
        for (int p2 = 0; p2 < 6 && char_index < strlen(filename); p2++) {
            lfn_entry.second_name_chunk[p2] = utf16_name[char_index++];
        }
        for (int p3 = 0; p3 < 2 && char_index < strlen(filename); p3++) {
            lfn_entry.third_name_chunk[p3] = utf16_name[char_index++];
        }

        entry_offset -= 32; // Move to the position for the LFN entry
        //fseek(fat->image, entry_offset, SEEK_SET);
        //fwrite(&lfn_entry, sizeof(LFN_t), 1, fat->image);
        diskmgr_write(obj->disk_nr, entry_offset, sizeof(LFN_t), &lfn_entry);
    }

    entry_offset += lfn_entry_count * 32;
    
    //fseek(fat->image, entry_offset, SEEK_SET);
    //fwrite(&entry, sizeof(DirectoryEntry_t), 1, fat->image);

    diskmgr_write(obj->disk_nr, entry_offset, sizeof(DirectoryEntry_t), &entry);

    fat32_flush(obj, fat);

    return new_cluster;
}

size_t fat32_write_experimental(fs_object_t* obj, fat_t* fat, size_t start_cluster, size_t file_size, size_t offset, size_t size, size_t* out_file_size, const char* buffer) {
    size_t bytes_written = 0;
    size_t cluster_size = fat->cluster_size;
    size_t current_cluster = start_cluster;

    // Calculate the offset within the first cluster
    size_t cluster_offset = offset % cluster_size;

    // Calculate the number of clusters needed for the write
    size_t initial_cluster_offset = offset / cluster_size;
    size_t end_offset = offset + size;
    size_t total_clusters_needed = (end_offset + cluster_size - 1) / cluster_size;

    // Traverse to the correct starting cluster based on the initial offset
    for (size_t i = 0; i < initial_cluster_offset; i++) {
        current_cluster = fat->fat_chain[current_cluster];
        if (current_cluster >= 0x0FFFFFF8) {
            // Reached the end of the chain unexpectedly
            current_cluster = 0;
            break;
        }
    }

    printf("%d okay\n", __LINE__);

    if (current_cluster == 0) {
        // Handle file extension if needed
        current_cluster = fat32_find_free_cluster(fat);
        if (current_cluster == 0) {
            // No free clusters available, cannot proceed
            *out_file_size = file_size;
            return 0;
        }
        fat->fat_chain[start_cluster] = current_cluster;
    }

    // Start writing data
    size_t buffer_offset = 0;
    while (buffer_offset < size) {
        // Calculate the number of bytes to write in the current cluster
        size_t write_size = cluster_size - cluster_offset;
        if (write_size > size - buffer_offset) {
            write_size = size - buffer_offset;
        }

        // Calculate the offset in the FAT image
        size_t write_offset = fat->cluster_base + (current_cluster * cluster_size) + cluster_offset;

        // Write the data
        //fseek(fat->image, write_offset, SEEK_SET);
        //fwrite(buffer + buffer_offset, 1, write_size, fat->image);
        
        diskmgr_write(obj->disk_nr, write_offset, write_size, buffer + buffer_offset);

        // Update tracking variables
        buffer_offset += write_size;
        bytes_written += write_size;
        cluster_offset = 0; // Only the first cluster might have an initial offset

        // Move to the next cluster if needed
        if (buffer_offset < size) {
            size_t next_cluster = fat->fat_chain[current_cluster];
            if (next_cluster >= 0x0FFFFFF8) {
                // Allocate a new cluster if needed
                next_cluster = fat32_find_free_cluster(fat);
                if (next_cluster == 0) {
                    // No more clusters available
                    break;
                }
                fat->fat_chain[current_cluster] = next_cluster;
                fat->fat_chain[next_cluster] = 0x0FFFFFF8; // Mark new cluster as end of the chain
            }
            current_cluster = next_cluster;
        }
    }

    // Update the file size if it has grown
    *out_file_size = offset + bytes_written > file_size ? offset + bytes_written : file_size;

    fat32_flush(obj, fat);

    return bytes_written;
}

void fat32_get_file_info_coords(fs_object_t* obj, fat_t* fat, uint32_t dir_cluster, const char* filename, size_t* out_cluster, size_t* out_offset) {
    uint32_t cluster_size = fat->cluster_size;
    uint32_t current_cluster = dir_cluster;
    size_t current_offset = 0;

    // Buffer to store the reconstructed LFN
    uint16_t in_name_buffer[256] = {0};
    size_t in_name_ptr = 0;
    char out_name_buffer[256] = {0};

    while (current_cluster < 0x0FFFFFF8) {
        char* cluster_data = calloc(1, cluster_size);
        //fat32_read_cluster_chain(obj, fat, current_cluster, false, cluster_data);
        fat32_read_one_cluster(obj, fat, current_cluster, cluster_data);
        qemu_log("Current cluster: %d", current_cluster);

        for (size_t offset = 0; offset < cluster_size; offset += 32) {
            DirectoryEntry_t* entry = (DirectoryEntry_t*)(cluster_data + offset);

            qemu_log("[%d] Entry %s", offset, (entry->attributes & ATTR_LONG_FILE_NAME) ? "LFN" : "ENTRY");

            if (entry->name[0] == 0x00) {
                // End of directory
                free(cluster_data);
                return;
            }

            if ((uint8_t)entry->name[0] == 0xE5) {
                // Deleted entry, skip
                continue;
            }

            if (entry->attributes & ATTR_LONG_FILE_NAME) {
                // Handle LFN entry
                LFN_t* lfn = (LFN_t*)(cluster_data + offset);
                in_name_ptr = 0; // Reset the name pointer

                for (int i = 0; i < 5; i++) {
                    if (lfn->first_name_chunk[i] == 0x0000) break;
                    in_name_buffer[in_name_ptr++] = lfn->first_name_chunk[i];
                }

                for (int i = 0; i < 6; i++) {
                    if (lfn->second_name_chunk[i] == 0x0000) break;
                    in_name_buffer[in_name_ptr++] = lfn->second_name_chunk[i];
                }

                for (int i = 0; i < 2; i++) {
                    if (lfn->third_name_chunk[i] == 0x0000) break;
                    in_name_buffer[in_name_ptr++] = lfn->third_name_chunk[i];
                }

                // If this is the last LFN part (indicated by bit 6 of the sequence number), process it
                if (lfn->attr_number & 0x40) {
                    utf16_to_utf8(in_name_buffer, in_name_ptr, (uint8_t*)out_name_buffer);

                    // Compare the reconstructed LFN with the target filename
                    qemu_log("`%s` vs `%s`", out_name_buffer, filename);
                    if (strcmp(out_name_buffer, filename) == 0) {
                        qemu_log("Clust: %d; Offset: %d", current_cluster, offset);
                        offset += (lfn->attr_number & ~0x40) * 32;

                        bool ow = offset / fat->cluster_size;
                        
                        if(ow) {
                            current_cluster++;
                            offset %= fat->cluster_size;
                        }

                        *out_cluster = current_cluster;
                        *out_offset = offset;
                        free(cluster_data);
                        return;
                    }

                    // Reset the buffers for the next file entry
                    memset(in_name_buffer, 0, sizeof(in_name_buffer));
                    memset(out_name_buffer, 0, sizeof(out_name_buffer));
                }
            } else if (!(entry->attributes & ATTR_LONG_FILE_NAME)) {
                // Handle short file name (SFN) entries (non-LFN case)
                char sfn[12] = {0};
                memcpy(sfn, entry->name, 8);
                if (entry->ext[0] != ' ') {
                    strcat(sfn, ".");
                    strncat(sfn, entry->ext, 3);
                }

                // Compare the SFN with the target filename
                if (strcmp(sfn, filename) == 0) {
                    *out_cluster = current_cluster;
                    *out_offset = offset;
                    free(cluster_data);
                    return;
                }
            }
        }

        // Move to the next cluster in the chain
        current_cluster = fat->fat_chain[current_cluster];
        free(cluster_data);
    }

    // If we reach here, the file was not found
    *out_cluster = 0;
    *out_offset = 0;
}

void fat32_get_file_info_coords_v2(fs_object_t* obj, fat_t* fat, uint32_t dir_cluster, const char* filename, size_t* out_cluster, size_t* out_offset) {
    size_t cluster_count = fat32_read_cluster_chain(obj, fat, dir_cluster, true, NULL);

    uint32_t* cluster_nrs = calloc(cluster_count, sizeof(uint32_t));
    fat32_read_cluster_numbers(fat, dir_cluster, cluster_nrs);

    for(int i = 0; i < cluster_count; i++) {
        qemu_log("=> %d", cluster_nrs[i]);
    }


    char* all_data = calloc(cluster_count, fat->cluster_size);
    qemu_log("[%d] DATA AT: %x", dir_cluster, all_data);

    fat32_read_cluster_chain(obj, fat, dir_cluster, false, all_data);
    
    for(int i = 0; i < cluster_count * fat->cluster_size; i++) {
        serial_printf(COM1, "%x", all_data[i]);
    }
    
    int offset = (cluster_count * fat->cluster_size) - 32;

    qemu_log("\nOFFSET: %d", offset);
    qemu_log("Base: %x", fat->cluster_base);


    char in_name[512] = {0};
    char out_name[512] = {0};
    size_t in_name_ptr = 0;
    
    size_t last_entry_cluster = 0;
    size_t last_entry_offset = 0;
    while(offset >= 0) {
        DirectoryEntry_t* entry = (DirectoryEntry_t*)(all_data + offset);

        qemu_log("Cur offset: %d", offset);
        qemu_log("Nimi: `%s`", entry->name);

        if(entry->name[0] == 0x00) {
            qemu_log("Ei nimi!");
            goto next;
        }

        if((uint8_t)entry->name[0] == 0xE5) {
            goto next;
        }

        if(!(entry->attributes & ATTR_LONG_FILE_NAME)) {
            size_t cnr = offset / fat->cluster_size;
            size_t cof = offset % fat->cluster_size;

            last_entry_cluster = cluster_nrs[cnr];
            last_entry_offset = cof;

            qemu_log("Last: %x; %d", last_entry_cluster, last_entry_offset);
        }
next:
        offset -= 32;
    }

    free(all_data);
    free(cluster_nrs);
}

DirectoryEntry_t fat32_read_file_info(fs_object_t* obj, fat_t* fat, size_t dir_clust, const char* file) {
    size_t out_clust, out_offset;
    DirectoryEntry_t de = {0};

    fat32_get_file_info_coords(obj, fat, dir_clust, file, &out_clust, &out_offset);

    size_t offset = fat->cluster_base + (out_clust * fat->cluster_size) + out_offset;
    //fseek(fat->image, offset, SEEK_SET);

    //fread(&de, sizeof(DirectoryEntry_t), 1, fat->image);
    diskmgr_read(obj->disk_nr, offset, sizeof(DirectoryEntry_t), &de);

    return de; 
}

void fat32_write_file_info(fs_object_t* obj, fat_t* fat, size_t dir_clust, const char* file, DirectoryEntry_t ent) {
    size_t out_clust, out_offset;

    fat32_get_file_info_coords(obj, fat, dir_clust, file, &out_clust, &out_offset);

    size_t offset = fat->cluster_base + (out_clust * fat->cluster_size) + out_offset;
    //fseek(fat->image, offset, SEEK_SET);

    //fwrite(&ent, sizeof(DirectoryEntry_t), 1, fat->image);
    diskmgr_write(obj->disk_nr, offset, sizeof(DirectoryEntry_t), &ent);
}

void fat32_write_size(fs_object_t* obj, fat_t* fat, size_t fp_cluster, size_t fp_offset, size_t size) {
    size_t offset = fat->cluster_base + (fp_cluster * fat->cluster_size) + fp_offset;
    printf("=====> %x\n", offset);
    //fseek(fat->image, offset, SEEK_SET);

    DirectoryEntry_t entry;
    //fread(&entry, sizeof(DirectoryEntry_t), 1, fat->image);
    
    diskmgr_read(obj->disk_nr, offset, sizeof(DirectoryEntry_t), &entry);

    //fseek(fat->image, offset, SEEK_SET);
    
    entry.file_size = size;

    //fwrite(&entry, sizeof(DirectoryEntry_t), 1, fat->image);
    diskmgr_write(obj->disk_nr, offset, sizeof(DirectoryEntry_t), &entry);
}

size_t fat32_write(fs_object_t* obj, fat_t* fat, const char* path, size_t offset, size_t size, const char* buffer) {
    size_t out_file_size;

    size_t cluster = fat32_search(obj, fat, path);

    if(cluster == 0) {
        qemu_log("Zero cluster\n");
        return 0;
    }

    size_t filesize = fat32_get_file_size(obj, fat, path);

    qemu_log("Prev fz: %d\n", filesize);

    fat32_write_experimental(obj, fat, cluster, filesize, offset, size, &out_file_size, buffer);

    size_t fcl, fof;


    const char* file = path + strlen(path);

    qemu_log("Parsing filename\n");

    while(*file != '/') {
        file--;
    }

    file++;

    qemu_log("Filename parsed\n");
    
    char* dirp = calloc((file - path) + 1, 1);

    memcpy(dirp, path, file - path);

    qemu_log("Path: `%s`\n", dirp);
    qemu_log("File: `%s`\n", file);

    size_t dir_cluster = fat32_search(obj, fat, dirp);

    qemu_log("Parent directory cluster: %d\n", dir_cluster);

    free(dirp);

    fat32_get_file_info_coords(obj, fat, dir_cluster, file, &fcl, &fof);
    fat32_get_file_info_coords_v2(obj, fat, dir_cluster, file, &fcl, &fof);

    qemu_log("Coords: Cluster: %d; Offset: %d", fcl, fof);

    fat32_write_size(obj, fat, fcl, fof, out_file_size);

    return 0;
}
