.section .init
	pop {fp}
	bx lr

.section .fini
	pop {fp}
	bx lr
