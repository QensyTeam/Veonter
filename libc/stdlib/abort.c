#include <stdio.h>
#include <stdlib.h>

__attribute__((__noreturn__))
void abort(void) {
#if defined(__is_libk)
	// TODO: kernel panic.
	printf("kernel: panic: abort()\n");
#else
	// TODO: SIGABRT.
	printf("abort()\n");
#endif
	while (1) { }
	__builtin_unreachable();
}
