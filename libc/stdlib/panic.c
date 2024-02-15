#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

char *panic_str;

__attribute__((__noreturn__))
void panic(char *s) {
  panic_str = s; 

  /* Sync HERE */

  printf("kernel panic: %s\n", s);
  
	while (1) { }
	__builtin_unreachable();
}
