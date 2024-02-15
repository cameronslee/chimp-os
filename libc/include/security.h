#ifndef _SECURITY_H
#define _SECURITY_H 1

#include <stdint.h>
#include <stdlib.h>

#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0xe2dee396
#else
#define STACK_CHK_GUARD 0x595e9fbd94fda766 

uintptr_t __stack_chk_guard;

#ifdef __cplusplus
extern "C" {
#endif

void __stack_chk_init(void);

__attribute__((noreturn))
void __stack_chk_fail(void);

#ifdef __cplusplus
}
#endif

#endif
#endif


