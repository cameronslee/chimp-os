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

void splash_screen() {
    const char* d = "Welcome to Chimp OS";
    terminal_writestring((const char *) d);
}

/* ======== Kernel ======== */
void kernel_main(void) {
    gdt_install();
    terminal_initialize();
    idt_install();
    isrs_install();
    irq_install();

    keyboard_install();

    // allow for IRQs 
    __asm__ __volatile__ ("sti"); 
    
    //install system timer
    timer_install();
    
    splash_screen();

    //event loop - FIXME as of right now, monotasking system
    while (1) {

    }

    //asm volatile ("1: jmp 1b"); // pseudo breakpoint

    //This should trigger a division by zero isr
    //__asm__  ("div %0" :: "r"(0));
}

