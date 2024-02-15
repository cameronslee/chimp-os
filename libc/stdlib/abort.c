#include <stdio.h>
#include <stdlib.h>

__attribute__((__noreturn__))
void abort(void) {
#if defined(__is_libk)
  panic("abort()");
#else
	// TODO: Abnormally terminate the process as if by SIGABRT.
  // TODO implement signal.h
	printf("abort()\n");
#endif
	while (1) { }
	__builtin_unreachable();
}
