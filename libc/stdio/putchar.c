#include <stdio.h>

#if defined(__is_libk)
#include <kernel/drv/tty.h>
#endif

int putchar(int ic) {
#if defined(__is_libk)
	char c = (char) ic;
	terminal_write(&c, sizeof(c));
#else
	// TODO: Системный вызов.
#endif
	return ic;
}
