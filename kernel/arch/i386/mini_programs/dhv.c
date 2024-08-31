#include <kernel/kernel.h>
#include <kernel/disk_manager.h>

extern disk_t disks[DISK_COUNT];

void dhv_program() {
    // Display the available disks and their capacities
    for (int i = 0; i < DISK_COUNT; i++) {
        if (disks[i].valid) {
            unsigned long long cap = diskmgr_get_capacity(i);
            printf("Disk number %d: %llu bytes (%llu KB, %llu MB)\n", i, cap, cap >> 10, cap >> 20);
        }
    }

    uint32_t disk = 0xff;  // Initialize the selected disk to an invalid value

    printf("Select disk: ");
    scanf("%d", &disk);  // Get the user's disk selection

    if (disk == 0xff) {
        printf("Invalid input.\n");
        return;
    }

    if (disk >= DISK_COUNT) {
        printf("Out of range.\n");
        return;
    }

    if (!disks[disk].valid) {
        printf("No disk\n");
        return;
    }

    // Get the capacity of the selected disk
    unsigned long long disk_capacity = diskmgr_get_capacity(disk);
    if (disk_capacity == 0) {
        printf("Disk capacity is zero, cannot read disk.\n");
        return;
    }

    // Ask for user confirmation to proceed
    printf("\n\tThe process might take a long time and cannot be interrupted.\nAre you sure you want to continue? (Y/N): ");
    char response = 0;
    scanf(" %c", &response);  // Read a single character input for the confirmation
    
    if (response != 'Y' && response != 'y') {
        printf("Process aborted by user.\n");
        return;  // Exit the program if the user did not confirm with 'Y' or 'y'
    }

    // Define a chunk size (e.g., 1 MB = 1024 * 1024 bytes)
    const unsigned long long CHUNK_SIZE = 1024 * 1024;  // 1 MB

    // Allocate a buffer for reading chunks
    char* buffer = calloc(CHUNK_SIZE, 1);
    if (!buffer) {
        printf("Memory allocation failed for disk buffer.\n");
        return;
    }

    const char* alphabet = "0123456789ABCDEF";

    // Read and print the disk in chunks
    for (unsigned long long offset = 0; offset < disk_capacity; offset += CHUNK_SIZE) {

        // Determine the size of the chunk to read (it might be less than CHUNK_SIZE for the last chunk)
        unsigned long long bytes_to_read = (disk_capacity - offset < CHUNK_SIZE) ? (disk_capacity - offset) : CHUNK_SIZE;

        // Display the progress and size of the chunk being read
        printf("Reading %llu bytes at offset %llu...\n", bytes_to_read, offset);

        // Read the current chunk into the buffer
        diskmgr_read(disk, offset, bytes_to_read, buffer);

        // Print the chunk in hexadecimal format
        for (unsigned long long i = 0; i < bytes_to_read; i++) {
            if ((i + offset) % 24 == 0) {  // 24 bytes per line
                printf("\n%015llx: ", offset + i);  // Print the offset address
            }

            uint8_t byte = buffer[i];
            printf("%c%c ", alphabet[(byte >> 4) & 0xf], alphabet[byte & 0xf]);
        }

        printf("\n");  // Print a newline after each chunk is read and printed
    }

    printf("Finished reading disk.\n");

    kfree(buffer);  // Free the allocated buffer
}
