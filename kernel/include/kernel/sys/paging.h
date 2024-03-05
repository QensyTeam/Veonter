#ifndef PAGING_H
#define PAGING_H

#include <kernel/sys/ports.h>
#include <kernel/sys/isr.h>
#include <kernel/panic.h>

/* Represents a page entry. */
typedef struct page {
  u32int present    : 1;   // Page present in memory
  u32int rw         : 1;   // Read-only if clear, readwrite if set
  u32int user       : 1;   // Supervisor level only if clear
  u32int accessed   : 1;   // Has the page been accessed since last refresh?
  u32int dirty      : 1;   // Has the page been written to since last refresh?
  u32int unused     : 7;   // Amalgamation of unused and reserved bits
  u32int frame      : 20;  // Frame address (shifted right 12 bits)
} page_t;

/* Represents a page table. */
typedef struct page_table {
  page_t pages[1024];
} page_table_t;

/* Represents a page directory. */
typedef struct page_directory{
  page_table_t *tables[1024];
    
  u32int tablesPhysical[1024];

  u32int physicalAddr;
} page_directory_t;

/* Sets up the environment, page directories etc and
   enables paging. */
void paging_initialize();

/* Causes the specified page directory to be loaded into the
   CR3 register. */
void switch_page_directory(page_directory_t *new);

/* Retrieves a pointer to the page required.
   If make == 1, if the page-table in which this page should
   reside isn't created, create it! */
page_t *get_page(u32int address, int make, page_directory_t *dir);

/* Handler for page faults. */
void page_fault(registers_t regs);

/* Deallocates a previously allocated frame. */
void free_frame(page_t*);

/* Allocate a new frame. */
void alloc_frame(page_t*, int, int);

/* Makes a copy of a page directory. */
page_directory_t *clone_directory(page_directory_t *src);

#endif