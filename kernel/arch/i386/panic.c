#include <kernel/panic.h>
#include <kernel/sys/ports.h>
#include <stdio.h>

__attribute__((noreturn))
extern void panic(const char *message, const char *file, u32int line)
{
    asm volatile("cli"); 

    printf("\nPANIC(");
    printf(message);
    printf(") at ");
    printf(file);
    printf(":");
    printf("%d", line);
    printf("\n");
    for(;;);
}

__attribute__((noreturn))
extern void panic_assert(const char *file, u32int line, const char *desc)
{
    asm volatile("cli"); 

    printf("\nASSERTION-FAILED(");
    printf(desc);
    printf(") at ");
    printf(file);
    printf(":");
    printf("%d", line);
    printf("\n");
    for(;;);
}
