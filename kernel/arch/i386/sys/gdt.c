#include <kernel/sys/gdt.h>
#include <stdlib.h>
#include <kernel/drv/tty.h>

void gdt_flush();

static uint64_t gdt[MAX_DESCRIPTORS];
static struct   gdtr gdtr;

int gdt_init() {
    const uint32_t COARSE_LIMIT = 0x000FFFFF;

	gdtr.m_limit = sizeof(uint64_t) * MAX_DESCRIPTORS-1;
	gdtr.m_base = (uint32_t)&gdt[0];

	gdt[0] = create_descriptor(0, 0, 0);	
    gdt[1] = create_descriptor(0, COARSE_LIMIT, (GDT_CODE_PL0));
    gdt[2] = create_descriptor(0, COARSE_LIMIT, (GDT_DATA_PL0));
    gdt[3] = create_descriptor(0, COARSE_LIMIT, (GDT_CODE_PL3));
    gdt[4] = create_descriptor(0, COARSE_LIMIT, (GDT_DATA_PL3));

	__asm__( "lgdt (%0)" :: "m" (gdtr) );
    gdt_flush();
	check();
    printf("GDT initialization completed successfully!\n");

	return 0;
}
uint64_t create_descriptor(uint32_t base, uint32_t limit, uint16_t flag)
{
    uint64_t descriptor;
 
    descriptor  =  limit       & 0x000F0000;         
    descriptor |= (flag <<  8) & 0x00F0FF00;        											     
    descriptor |= (base >> 16) & 0x000000FF;         
    descriptor |=  base        & 0xFF000000;         
    descriptor <<= 32;
    descriptor |= base  << 16;                       
    descriptor |= limit  & 0x0000FFFF;               

	return descriptor;
}