.set ALIGN,    1<<0             
.set MEMINFO,  1<<1             
.set VBE,      1<<2             
.set FLAGS,    ALIGN | MEMINFO | VBE  
.set MAGIC,    0x1BADB002       
.set CHECKSUM, -(MAGIC + FLAGS) 

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
.long 0, 0, 0, 0, 0
.long 0
.long 1024, 768, 32

.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 
stack_top:

.section .text
.global _start
.type _start, @function
_start:
    movl $stack_top, %esp
    
    pushl %eax
    pushl %ebx

	call kernel_early

	call kernel_main

	cli
.Lhang:
	hlt
	jmp .Lhang
	
.size _start, . - _start
