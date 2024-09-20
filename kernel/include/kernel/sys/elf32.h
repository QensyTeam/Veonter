#ifndef ELF32_EHDR_H
#define ELF32_EHDR_H

#include <stdint.h>

typedef struct {
    uint8_t  e_ident[16];    // Идентификатор ELF
    uint16_t e_type;         // Тип файла (например, исполняемый, библиотека)
    uint16_t e_machine;      // Архитектура (например, x86)
    uint32_t e_version;      // Версия ELF
    uint32_t e_entry;        // Адрес точки входа
    uint32_t e_phoff;        // Смещение заголовков программы
    uint32_t e_shoff;        // Смещение заголовков секции
    uint32_t e_flags;        // Флаги
    uint16_t e_ehsize;       // Размер ELF заголовка
    uint16_t e_phentsize;    // Размер заголовка программы
    uint16_t e_phnum;        // Количество заголовков программы
    uint16_t e_shentsize;    // Размер заголовка секции
    uint16_t e_shnum;        // Количество заголовков секции
    uint16_t e_shstrndx;     // Индекс секции со строками
} Elf32_Ehdr;

#endif // ELF32_EHDR_H
