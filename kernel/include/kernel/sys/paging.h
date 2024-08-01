#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stdbool.h>

#define PAGE_DIRECTORY_SIZE 1024
#define PAGE_TABLE_SIZE 1024

typedef struct {
    uint32_t present    : 1;  
    uint32_t rw         : 1;   
    uint32_t user       : 1;   
    uint32_t accessed   : 1;   
    uint32_t dirty      : 1;   
    uint32_t unused     : 7;   
    uint32_t frame      : 20;  
} page_table_entry_t;

typedef struct {
    page_table_entry_t pages[PAGE_TABLE_SIZE];
} page_table_t;

typedef struct {
    page_table_t* tables[PAGE_DIRECTORY_SIZE];
    uint32_t tablesPhysical[PAGE_DIRECTORY_SIZE]; 
    uint32_t physicalAddr; 
} page_directory_t;

page_directory_t* paging_create_page_directory();

page_table_t* paging_create_page_table();

void paging_map_page(page_directory_t* directory, uint32_t virtualAddr, uint32_t physicalAddr, bool isUser, bool isWritable);

void paging_switch_page_directory(page_directory_t* directory);

void paging_init();

#endif /* PAGING_H */
