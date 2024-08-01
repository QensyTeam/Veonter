#ifndef _KERNEL_MULTIBOOT_H
#define _KERNEL_MULTIBOOT_H

#define MULTIBOOT_HEADER_MAGIC          0x1BADB002

#ifdef __ELF__
# define MULTIBOOT_HEADER_FLAGS         0x00000003
#else
# define MULTIBOOT_HEADER_FLAGS         0x00010003
#endif

#define MULTIBOOT_BOOTLOADER_MAGIC      0x2BADB002

#define STACK_SIZE                      0x4000
#define MULTIBOOT_FLAG_VBE     0x400

#ifdef HAVE_ASM_USCORE
# define EXT_C(sym)                     _ ## sym
#else
# define EXT_C(sym)                     sym
#endif

#ifndef ASM

typedef struct multiboot_header
{
    unsigned long magic;
    unsigned long flags;
    unsigned long checksum;
    unsigned long header_addr;
    unsigned long load_addr;
    unsigned long load_end_addr;
    unsigned long bss_end_addr;
    unsigned long entry_addr;
} multiboot_header_t;

typedef struct {
    uint16_t attributes;
    uint8_t winA, winB;
    uint16_t granularity;
    uint16_t winsize;
    uint16_t segmentA, segmentB;
    uint32_t realFctPtr;
    uint16_t pitch; 

    uint16_t Xres, Yres;
    uint8_t Wchar, Ychar, planes, bpp, banks;
    uint8_t memory_model, bank_size, image_pages;
    uint8_t reserved0;

    uint8_t red_mask, red_position;
    uint8_t green_mask, green_position;
    uint8_t blue_mask, blue_position;
    uint8_t rsv_mask, rsv_position;
    uint8_t directcolor_attributes;

    uint32_t physbase;  
    uint32_t reserved1;
    uint16_t reserved2;
} vbe_mode_info_t;

typedef struct aout_symbol_table
{
    unsigned long tabsize;
    unsigned long strsize;
    unsigned long addr;
    unsigned long reserved;
} aout_symbol_table_t;

typedef struct elf_section_header_table
{
    unsigned long num;
    unsigned long size;
    unsigned long addr;
    unsigned long shndx;
} elf_section_header_table_t;

typedef struct multiboot_info
{
    unsigned long flags;
    unsigned long mem_lower;
    unsigned long mem_upper;
    unsigned long boot_device;
    unsigned long cmdline;
    unsigned long mods_count;
    unsigned long mods_addr;
    union
    {
        aout_symbol_table_t aout_sym;
        elf_section_header_table_t elf_sec;
    } u;
    unsigned long mmap_length;
    unsigned long mmap_addr;
    unsigned long drives_length;
    unsigned long drives_addr;
    unsigned long config_table;
    unsigned long boot_loader_name;
    unsigned long apm_table;
    unsigned long vbe_control_info;
    unsigned long vbe_mode_info;
    unsigned short vbe_mode;
    unsigned short vbe_interface_seg;
    unsigned short vbe_interface_off;
    unsigned short vbe_interface_len;
    unsigned short framebuffer_pitch;
    unsigned short framebuffer_width;
    unsigned short framebuffer_height;
} multiboot_info_t;

typedef struct module
{
    unsigned long mod_start;
    unsigned long mod_end;
    unsigned long string;
    unsigned long reserved;
} module_t;

typedef struct memory_map
{
    unsigned long size;
    unsigned long base_addr_low;
    unsigned long base_addr_high;
    unsigned long length_low;
    unsigned long length_high;
    unsigned long type;
} memory_map_t;

struct multiboot_tag_module
{
  uint32_t type;
  uint32_t size;
  uint32_t mod_start;
  uint32_t mod_end;
  char cmdline[0];
}__attribute__((packed));

typedef struct multiboot_tag_module multiboot_tag_module_t;

#endif /* ! ASM */

#endif /* MULTIBOOT_H */