.global _reset
_reset:

mrs	r0, cpsr		/* disable interrupts */
orr	r0, r0, #0xc0
msr	cpsr, r0

ldr sp, =0x50000000

bl clear_bss
bl kernel_early
bl kernel_main

spin: b spin

.globl _ivt
_ivt:
ldr pc, _reset_h
ldr pc, _unused_handler_h
_reset_h:                           .word   _reset
_unused_handler_h:                  .word   _reset

clear_bss:
	movw	r3, #:lower16:__bss_start__
	movt	r3, #:upper16:__bss_start__
	movw	r2, #:lower16:__bss_end__
	movt	r2, #:upper16:__bss_end__
	mov	r1, #0
loop:	cmp	r3, r2
	strcc	r1, [r3]
	addcc	r3, r3, #4
	bcc	loop
	bx	lr

.section        .note.GNU-stack,"",%progbits
