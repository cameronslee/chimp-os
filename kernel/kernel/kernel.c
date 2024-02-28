#include <stdio.h>
#include <string.h>
#include <security.h>

#include <kernel/system.h>
#include <kernel/tty.h>
#include <kernel/idt.h>
#include <kernel/gdt.h>
#include <kernel/isr.h>
#include <kernel/irq.h>
#include <kernel/pit.h>
#include <kernel/keyboard.h>


/* ======== Kernel ======== */
void kernel_main(void) {
    gdt_install();
    idt_install();
    isrs_install();
    irq_install();
    timer_install();

    terminal_initialize();
    keyboard_install();

    // TODO implement system timer

    __asm__ __volatile__ ("sti"); 

    // wait 5 seconds
    timer_wait(500);

    const char* d = "                               Welcome to Chimp OS\n";
    terminal_writestring((const char *) d);

    //asm volatile ("1: jmp 1b"); // pseudo breakpoint

    //This should trigger a division by zero isr
    //__asm__  ("div %0" :: "r"(0));
}

