#ifndef _KERNEL_IRQ_H
#define _KERNEL_IRQ_H

#include <kernel/system.h>

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

/* Custom IRQ handler installer */
void irq_install_handler(int irq, void (*handler)(struct regs *r));

/* Reset the handler for a given IRQ */
void irq_uninstall_handler(int irq);

/* Send signal to Programmable Interrupt Controller (8259/PIC) in
 * order to remap irq0 - irq15 to IDT entries 32 - 47 (default is 0-7)
 */
void irq_remap(void);

void irq_install();
void irq_handler(struct regs *r);

#endif
