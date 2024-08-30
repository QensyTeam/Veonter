// FAT32 v1.0 driver originally written by /> NDRAEY (c) 2024
// Modifications and some fixes by Qensy (c) 2024
//
// Built for Veonter

#include <kernel/fs/fat32/lfn.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void LFN2SFN(const char* in_filename, char* out_filename) {
    int filename_len = strlen(in_filename);
    char* temp = calloc(filename_len, 1);

    memset(out_filename, ' ', 11);

    size_t space_count = 0;

    for(int i = 0; i < filename_len; i++) {
        if(in_filename[i] == ' ') {
            space_count++;
            continue;
        }

        temp[i - space_count] = (char)toupper(in_filename[i]);
    }

    const char* temp_end = temp + filename_len - space_count;

    const char* extension = strchr(temp, '.');
    if(extension != NULL) {
        int ext_len = temp_end - extension - 1;

        memcpy(out_filename + 11 - ext_len, extension + 1, ext_len);

        int name_len = extension - temp;

        if(name_len > 8) {
            memcpy(out_filename, temp, 6);
            out_filename[6] = '~';
            out_filename[7] = '1';
        } else {
            memcpy(out_filename, temp, name_len);
        }
    }

    free(temp);
}

uint8_t lfn_checksum(const char sfn[11]) {
    uint8_t sum = 0;

    for(int i = 11; i != 0; i--) {
        sum = (sum >> 1) + ((sum & 1) << 7) + *sfn++;
    }

    return sum;
}
