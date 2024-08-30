#include <kernel/kernel.h>
#include <stddef.h>
#include <stdbool.h>

page_directory_t* paging_create_page_directory() {
    page_directory_t* directory = kmalloc(sizeof(page_directory_t));

    for (int i = 0; i < PAGE_DIRECTORY_SIZE; ++i) {
        directory->tables[i] = NULL;
        directory->tablesPhysical[i] = 0;
    }

    directory->physicalAddr = (uint32_t)directory->tablesPhysical;

    return directory;
}

page_table_t* paging_create_page_table() {
    page_table_t* table = kmalloc(sizeof(page_table_t));

    for (int i = 0; i < PAGE_TABLE_SIZE; ++i) {
        table->pages[i].frame = 0;
        table->pages[i].present = 0;
        table->pages[i].rw = 0;
        table->pages[i].user = 0;
        table->pages[i].accessed = 0;
        table->pages[i].dirty = 0;
        table->pages[i].unused = 0;
    }

    return table;
}

void paging_map_page(page_directory_t* directory, uint32_t virtualAddr, uint32_t physicalAddr, bool isUser, bool isWritable) {
    uint32_t directoryIndex = virtualAddr >> 22;
    uint32_t tableIndex = (virtualAddr >> 12) & 0x03FF;

    if (!directory->tables[directoryIndex]) {
        directory->tables[directoryIndex] = paging_create_page_table();
        directory->tablesPhysical[directoryIndex] = (uint32_t)directory->tables[directoryIndex] | 0x07;
    }

    page_table_t* table = directory->tables[directoryIndex];
    table->pages[tableIndex].frame = physicalAddr >> 12;
    table->pages[tableIndex].present = 1;
    table->pages[tableIndex].rw = isWritable ? 1 : 0;
    table->pages[tableIndex].user = isUser ? 1 : 0;
}

void paging_switch_page_directory(page_directory_t* directory) {
    __asm__ volatile("mov %0, %%cr3" : : "r"(directory->physicalAddr));
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; 
    __asm__ volatile("mov %0, %%cr0" : : "r"(cr0));
}

void paging_init() {
    page_directory_t* directory = paging_create_page_directory();
    paging_switch_page_directory(directory);
}

