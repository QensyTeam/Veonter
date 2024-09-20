#include <kernel/sys/elf32.h>
#include <kernel/drv/tty.h>
#include <kernel/multiboot.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int parse_elf32(void *elf_data, size_t size) {
    if (size < sizeof(Elf32_Ehdr)) {
        printf("Error: file is too small for ELF header.\n");
        return;
    }

    Elf32_Ehdr *header = (Elf32_Ehdr*)elf_data;

    if (memcmp(header->e_ident + 1, "ELF", 3) != 0) {
        printf("Error: file is not an ELF file.\n");
        printf("%s\n", header->e_ident);
        return -1;
    }

    printf("File type: %u\n", header->e_type);
    printf("Architecture: %u\n", header->e_machine);
    printf("ELF version: %u\n", header->e_version);
    printf("Entry point address: 0x%x\n", header->e_entry);
    printf("Program header offset: %u\n", header->e_phoff);
    printf("Section header offset: %u\n", header->e_shoff);
    printf("Number of program headers: %u\n", header->e_phnum);
    printf("Number of section headers: %u\n", header->e_shnum);

    return header->e_entry + elf_data;
}

void test_elf(uint32_t *addr, uint32_t count) {
    printf("Mods addr: %x, count %u\n", addr, count);
    module_t *mod = (module_t*)addr;

    for (uint32_t i = 0; i < count; i++) {
        printf("Module %u:\n", i);
        printf("  mod_start: %x\n", mod[i].mod_start);
        printf("  mod_end: %x\n", mod[i].mod_end);
        printf("  string: %s\n", (char*)mod[i].string);
        printf("  reserved: %x\n", mod[i].reserved);
        parse_elf32(mod[i].mod_start, (size_t)((uint32_t)mod[i].mod_end - (uint32_t)mod[i].mod_start));
    }
}