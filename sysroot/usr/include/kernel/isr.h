#ifndef _KERNEL_ISR_H
#define _KERNEL_ISR_H

#include <kernel/idt.h>
#include <kernel/system.h>

void isrs_install();

void fault_handler(struct regs *r);
#endif
