#include <stdio.h>

#if defined(__is_libk)
#include <kernel/sys/gui/shell.h>
#endif

int putchar(int ic) {
#if defined(__is_libk)
    shell_putchar(ic);
#else
	// TODO: Системный вызов.
#endif
	return ic;
}
