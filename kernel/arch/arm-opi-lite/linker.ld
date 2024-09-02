SECTIONS
{
	. = 0x42000000;  /* Kernel must load in U-Boot at the SAME address */

	.text : {
		*(.text)
		*(.text.*)
	}
	
	.rodata : {
		*(.rodata)
		*(.rodata.*)
	}
	
	.data : {
		*(.data)
		*(.data.*)
	}

	. = ALIGN(4096);		/* Init code and data */

	__bss_start__ = .;
	
	.bss : {
		*(COMMON)
		*(.bss*)
	}

	__bss_end__ = .;
}
