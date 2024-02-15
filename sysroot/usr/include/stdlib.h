#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort(void);

/* Kernel Panic */
__attribute__((__noreturn__))
void panic(char *s);

#ifdef __cplusplus
}
#endif

#endif
