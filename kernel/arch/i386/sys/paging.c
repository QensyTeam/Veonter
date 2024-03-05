#include <kernel/sys/paging.h>
#include <kernel/sys/kheap.h>
#include <kernel/sys/isr.h> //for register_interrupt_hander
#include <kernel/sys/ordered_array.h>
#include <stdio.h> //for printf
#include <string.h> //for memset
/* The kernel's page directory. */
page_directory_t *kernel_directory=0;

/* The current page directory. */
page_directory_t *current_directory=0;

/* A bitset of frames - used or free. */
u32int *frames;

/* The total number of frames available in ram */
u32int nframes;

/* The address where the heap is initialized. Defined in mm.c */
extern u32int placement_address;

/* The kernel heap, defined in kheap.c */
extern heap_t *kheap;

/* defined in mm.c - The amount of memory usable for allocation, after the kernel. */
extern int memUsable;

/* Macros used in the bitset algorithms. */
#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))


/* Marks a frame as used in the frames[] bitset. */
static void set_frame(u32int frame_addr){
  u32int frame = frame_addr/0x1000;
  u32int idx = INDEX_FROM_BIT(frame);
  u32int off = OFFSET_FROM_BIT(frame);
  frames[idx] |= (0x1 << off);
}

/* Marks a frame as unused in the frames[] bitset. */
static void clear_frame(u32int frame_addr){
  u32int frame = frame_addr/0x1000;
  u32int idx = INDEX_FROM_BIT(frame);
  u32int off = OFFSET_FROM_BIT(frame);
  frames[idx] &= ~(0x1 << off);
}

/* Returns index of the first unused frame in the frame[] bitset. */
static u32int first_frame(){
  u32int i, j;
  for (i = 0; i < INDEX_FROM_BIT(nframes); i++){
    if (frames[i] != 0xFFFFFFFF){ // nothing free, exit early.
      // at least one bit is free here.
      for (j = 0; j < 32; j++){
        u32int toTest = 0x1 << j;
        if (!(frames[i]&toTest)){
          return i*4*8+j;
        }
      }
    }
  }
  return -1;
}

/* Allocate a new frame */
void alloc_frame(page_t *page, int is_kernel, int is_writeable){
  if (page->frame != 0){
    return;
  }else{
    u32int idx = first_frame();
    if (idx == (u32int)-1){
      // PANIC! no free frames!!
      PANIC("NO Free Frames!");
    }
    set_frame(idx*0x1000);
    page->present = 1;
    page->rw = (is_writeable)?1:0;
    page->user = (is_kernel)?0:1;
    page->frame = idx;
  }
}

// Deallocated a previously allocated frame. */
void free_frame(page_t *page){
  u32int frame;
  if (!(frame=page->frame)){
    return;
  }else{
    clear_frame(frame);
    page->frame = 0x0;
  }
}

/* Initializes the paging system. */
void paging_initialize(){
    u32int mem_end_page = 0x1000000;

    nframes = mem_end_page / 0x1000;
    frames = (u32int*)kmalloc_a(INDEX_FROM_BIT(nframes));
    memset(frames, 0, INDEX_FROM_BIT(nframes));
        
    kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));

    kernel_directory->physicalAddr = (u32int)kernel_directory->tablesPhysical;//added for multitasking
    unsigned int i = 0;
    for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000){
        get_page(i, 1, kernel_directory);
    }

    i = 0;
    while (i < placement_address+0x1000){
        alloc_frame( get_page(i, 1, kernel_directory), 0, 0);
        i += 0x1000;
    }

    for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000){
        alloc_frame( get_page(i, 1, kernel_directory), 0, 0);
    }

    install_irq_handler(14, page_fault);

    switch_page_directory(kernel_directory);

    // Initialise the kernel heap.
    kheap = create_heap(KHEAP_START, KHEAP_START+KHEAP_INITIAL_SIZE, 0xCFFFF000, 0, 0);
    
    
    
}

void switch_page_directory(page_directory_t *dir)
{
    current_directory = dir;
    asm volatile("mov %0, %%cr3":: "r"(&dir->tablesPhysical));
    u32int cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging!
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}

page_t *get_page(u32int address, int make, page_directory_t *dir){
  // Turn the address into an index.
  address /= 0x1000;
  
  // Find the page table containing this address.
  u32int table_idx = address / 1024;
  if (dir->tables[table_idx]){ // If this table is already assigned
    return &dir->tables[table_idx]->pages[address%1024];
  }else if(make){
    u32int tmp;
    dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
    memset(dir->tables[table_idx], 0, 0x1000);
    dir->tablesPhysical[table_idx] = tmp | 0x7; // PRESENT, RW, US.
    //printf("\n &dir->tablesPhysical[0]: 0x%x:", &dir->tablesPhysical[0]); // test
    return &dir->tables[table_idx]->pages[address%1024];
  }else{
    return 0;
  }
}

void page_fault(registers_t regs)
{
    u32int faulting_address;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

    int present   = !(regs.err_code & 0x1); // Page not present
    int rw = regs.err_code & 0x2;           // Write operation?
    int us = regs.err_code & 0x4;           // Processor was in user-mode?
    int reserved = regs.err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
    int id = regs.err_code & 0x10;          // Caused by an instruction fetch?
    id = id;

    printf("\nPage fault! ( ");
    if (present) {printf("present ");}
    if (rw) {printf("read-only ");}
    if (us) {printf("user-mode ");}
    if (reserved) {printf("reserved ");}
    printf(")\n at 0x");
    printf("%x", faulting_address);
    printf("\n");
    
    if(!present && !rw && !us){
        printf("\nSupervisory process tried to read a non-present page entry.");
    }else if(!present && !rw && us){
        printf("\nSupervisory process tried to read a page and cause a protection fault.");
    }else if(!present && rw && !us){
        printf("\nSupervisory process tried to write to a non-present page entry.");
    }else if(!present && rw && us){
        printf("\nSupervisory process tried to write a page and caused a protection fault.");
    }else if(present && !rw && !us){
        printf("\nUser Process tried to read a non-present page entry.");
    }else if(present && !rw && us){
        printf("\nUser process tried to read a page and caused a protection fault.");
    }else if(present && rw && !us){
        printf("\nUer process tried to write to a non-present page entry.");
    }else if(present && rw && us){
        printf("\nUser process tried to write a page and caused a protection fault.");
    }
    printf("\n"); 
    
    PANIC("Page fault");
}
