#ifndef _KERNEL_KEYBOARD_H
#define _KERNEL_KEYBOARD_H

#include <kernel/system.h>

void keyboard_handler(struct regs *r);
void keyboard_install();

#endif
