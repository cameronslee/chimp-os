#ifndef _SIGNAL_H
#define _SIGNAL_H 1

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

// TODO implement
__attribute__((__noreturn__))
void signal();

__attribute__((__noreturn__))
void psignal();

#ifdef __cplusplus
}
#endif

#endif
