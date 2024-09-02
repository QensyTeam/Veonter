.section .init
.global _init
.type _init, %function
_init:
    push {fp, lr}         // fp - like `ebp` in x86, lr - like `esp` in x86
    add fp, sp, #0        // fp = sp + 0    (eqiv to mov)

    // ...

    pop {fp, lr}          // Restore frame pointer and link register
    bx lr                 // like `ret` from x86

.section .fini
.global _fini
.type _fini, %function
_fini:
    push {fp, lr}
    add fp, sp, #0

    // ...

    pop {fp, lr}
    bx lr
