#include <stdio.h>
#include <string.h>
#include <security.h>

#include <kernel/tty.h>

#define NUM_COLORS 16

void kernel_main(void) {
	terminal_initialize();
  const char* d = "                               Welcome to Chimp OS\n";
  terminal_writestring(d);
}

