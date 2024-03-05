#include <kernel/panic.h>
#include <kernel/sys/ports.h>
#include <stdio.h>

extern void panic(const char *message, const char *file, u32int line)
{
    // We encountered a massive problem and have to stop.
    asm volatile("cli"); // Disable interrupts.

    printf("\nPANIC(");
    printf(message);
    printf(") at ");
    printf(file);
    printf(":");
    printf("%d", line);
    printf("\n");
    // Halt by going into an infinite loop.
    for(;;);
}

extern void panic_assert(const char *file, u32int line, const char *desc)
{
    // An assertion failed, and we have to panic.
    asm volatile("cli"); // Disable interrupts.

    printf("\nASSERTION-FAILED(");
    printf(desc);
    printf(") at ");
    printf(file);
    printf(":");
    printf("%d", line);
    printf("\n");
    // Halt by going into an infinite loop.
    for(;;);
}