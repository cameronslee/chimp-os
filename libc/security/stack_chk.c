#include <security.h>

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

__attribute__((noreturn))
void __stack_chk_fail(void)
{
#if __STDC_HOSTED__
	abort();
#elif __is_chimpos_kernel
	panic("Stack smashing detected");
#endif
}
