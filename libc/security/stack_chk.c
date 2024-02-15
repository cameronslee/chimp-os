#include <security.h>

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

__attribute__((noreturn))
void __stack_chk_init(void)
{

}

__attribute__((noreturn))
void __stack_chk_fail(void)
{
// defined with 1 if the compilers target is a hosted env.
#if __STDC_HOSTED__
	abort();
#elif __is_chimpos_kernel
	panic("Stack smashing detected");
#endif
}
