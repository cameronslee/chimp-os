#include <stdio.h>
#include <string.h>
#include <security.h>

#include <kernel/tty.h>

#define NUM_COLORS 16
#define SYS_FREQ 100

/* ======== Helpers ======== */
unsigned char inportb (unsigned short _port)
{
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outportb(unsigned short port, unsigned char value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

char* itoa(int value, char *str, int base) {
    // Handle the case where base is not supported or out of range
    if (base < 2 || base > 36) {
        *str = '\0';
        return str;
    }

    char *ptr = str;
    int quotient = value;

    // Handle the case where the value is zero separately
    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return str;
    }

    // Handle negative numbers
    if (value < 0 && base == 10) {
        *ptr++ = '-';
        quotient = -value;
    }

    // Convert the value to the specified base
    while (quotient != 0) {
        int remainder = quotient % base;
        *ptr++ = (remainder < 10) ? (remainder + '0') : (remainder - 10 + 'a');
        quotient /= base;
    }

    // Add null terminator
    *ptr = '\0';

    // Reverse the string
    char *begin = str;
    char *end = ptr - 1;
    while (begin < end) {
        char temp = *begin;
        *begin++ = *end;
        *end-- = temp;
    }

    return str;
}

/* ======== IDT ======== */
/*       IDT Entry
 * -----------------------
 * |7  6    5  4        0|
 * -----------------------
 * |P  DPL     always    |
 * -----------------------
 *
 * P: segment present? 1 = yes
 * DPL: Ring (0-3)
 * Always: 01110 (lower bits set to 01110)
 *
*/
struct idt_entry
{
    unsigned short base_lo;
    unsigned short sel;        /* kernel segment */
    unsigned char always0;     /* ALWAYS set to 0! */
    unsigned char flags;       
    unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

/* IDT of 256 entries, first 32 are reserved by intel 
 * If any undefined IDT entry is hit, it normally
 *  will cause an "Unhandled Interrupt" exception. Any descriptor
 *  for which the 'presence' bit is cleared (0) will generate an
 *  "Unhandled Interrupt" exception
*/
struct idt_entry idt[256];
struct idt_ptr idtp;

/* lives in boot.S */
extern void idt_load();

void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags) 
{
    // apply bitmask to preserve lower 16 bits
    idt[num].base_lo = (base & 0xFFFF); 
    // shift 16 bits, apply bit mask for upper 16 bits for safety
    idt[num].base_hi = ((base >> 16) & 0xFFFF);

    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;

}

void idt_install() 
{
    /* Setup ptr */
    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = (unsigned int) &idt;

    /* Clear out IDT*/
    memset(&idt, 0, sizeof(struct idt_entry) * 256);

    /* TODO Add any new ISRs to the IDT here with idt_set_gate */


    /* Points the processor's internal register to the new IDT */
    idt_load();
}

/* ======== GDT ======== */
//TODO was tired of linking errors so implemented here. need to move
/*                        Segment Descriptor 
 * --------------------------------------------------------------
 * |63        56  55    52  51    48  47        40  39        32|                 
 * --------------------------------------------------------------
 * |Base          Flags     Limit     Access        Base        |
 * |31        24  3     0   19    16  7         0   23        16|
 * --------------------------------------------------------------
 * |31                            16  15                       0|
 * --------------------------------------------------------------
 * |Base                              Limit                     |
 * |15                            0   15                       0|
 * --------------------------------------------------------------
 *
*/
struct gdt_entry
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed)); //packed prevents compiler optimizations. 

struct gdt_ptr
{
    unsigned short limit; // size of the gdt - 1
    unsigned int base; // linear address of the gdt
} __attribute__((packed));

struct gdt_entry gdt[3];
struct gdt_ptr gp;

/* asm routine lives in boot.S */
extern void gdt_flush();

/* Setup a descriptor in the Global Descriptor Table */
void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
{
    /* Setup the descriptor base address */
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    /* Setup the descriptor limits */
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    /* Finally, set up the granularity and access flags */
    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

void gdt_install()
{
    /* GDT ptr*/
    gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gp.base = (unsigned int) &gdt;

    /* NULL descriptor */
    gdt_set_gate(0, 0, 0, 0, 0);

    /* The second entry is our Code Segment. The base address
    *  is 0, the limit is 4GBytes, it uses 4KByte granularity,
    *  uses 32-bit opcodes, and is a Code Segment descriptor.
    */
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    /* The third entry is our Data Segment. It's EXACTLY the
    *  same as our code segment, but the descriptor type in
    *  this entry's access byte says it's a Data Segment */
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    /* Flush out the old GDT and install the new */
    gdt_flush();
}
		

/* ======== ISR HANDLES ======== */
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

void isrs_install()
{
    idt_set_gate(0, (unsigned)isr0, 0x08, 0x8E);
    idt_set_gate(1, (unsigned)isr1, 0x08, 0x8E);
    idt_set_gate(2, (unsigned)isr2, 0x08, 0x8E);
    idt_set_gate(3, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(4, (unsigned)isr4, 0x08, 0x8E);
    idt_set_gate(5, (unsigned)isr5, 0x08, 0x8E);
    idt_set_gate(6, (unsigned)isr6, 0x08, 0x8E);
    idt_set_gate(7, (unsigned)isr7, 0x08, 0x8E);

    idt_set_gate(8, (unsigned)isr8, 0x08, 0x8E);
    idt_set_gate(9, (unsigned)isr9, 0x08, 0x8E);
    idt_set_gate(10, (unsigned)isr10, 0x08, 0x8E);
    idt_set_gate(11, (unsigned)isr11, 0x08, 0x8E);
    idt_set_gate(12, (unsigned)isr12, 0x08, 0x8E);
    idt_set_gate(13, (unsigned)isr13, 0x08, 0x8E);
    idt_set_gate(14, (unsigned)isr14, 0x08, 0x8E);
    idt_set_gate(15, (unsigned)isr15, 0x08, 0x8E);

    idt_set_gate(16, (unsigned)isr16, 0x08, 0x8E);
    idt_set_gate(17, (unsigned)isr17, 0x08, 0x8E);
    idt_set_gate(18, (unsigned)isr18, 0x08, 0x8E);
    idt_set_gate(19, (unsigned)isr19, 0x08, 0x8E);
    idt_set_gate(20, (unsigned)isr20, 0x08, 0x8E);
    idt_set_gate(21, (unsigned)isr21, 0x08, 0x8E);
    idt_set_gate(22, (unsigned)isr22, 0x08, 0x8E);
    idt_set_gate(23, (unsigned)isr23, 0x08, 0x8E);

    idt_set_gate(24, (unsigned)isr24, 0x08, 0x8E);
    idt_set_gate(25, (unsigned)isr25, 0x08, 0x8E);
    idt_set_gate(26, (unsigned)isr26, 0x08, 0x8E);
    idt_set_gate(27, (unsigned)isr27, 0x08, 0x8E);
    idt_set_gate(28, (unsigned)isr28, 0x08, 0x8E);
    idt_set_gate(29, (unsigned)isr29, 0x08, 0x8E);
    idt_set_gate(30, (unsigned)isr30, 0x08, 0x8E);
    idt_set_gate(31, (unsigned)isr31, 0x08, 0x8E);
}

const char *exception_messages[] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};


/* Stack architecture once ISR runs */
struct regs
{
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pushw' */
    unsigned int int_no, err_code;    /* pushw #' and err code*/
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};
/*  
 *  Upon fault, endless loop. 
 *
 *  All ISRs disable interrupts while they are being
 *  serviced as a 'locking' mechanism to prevent an IRQ from
 *  happening and messing up kernel data structures 
*/

void fault_handler(struct regs *r)
{
    if (r->int_no < 32)
    {
        puts(exception_messages[r->int_no]);
        puts(" Exception. System Halted!\n");
        for (;;);
    }
}

/* ======== IRQ HANDLES ======== */

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

// array of func ptrs for custom IRQ handles
void *irq_routines[16] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

/* Custom IRQ handler installer */
void irq_install_handler(int irq, void (*handler)(struct regs *r))
{
    irq_routines[irq] = handler;
}

/* Reset the handler for a given IRQ */
void irq_uninstall_handler(int irq)
{
    irq_routines[irq] = 0;
}

/* Send signal to Programmable Interrupt Controller (8259/PIC) in
 * order to remap irq0 - irq15 to IDT entries 32 - 47 (default is 0-7)
 */
void irq_remap(void)
{
    outportb(0x20, 0x11);
    outportb(0xA0, 0x11);
    outportb(0x21, 0x20);
    outportb(0xA1, 0x28);
    outportb(0x21, 0x04);
    outportb(0xA1, 0x02);
    outportb(0x21, 0x01);
    outportb(0xA1, 0x01);
    outportb(0x21, 0x0);
    outportb(0xA1, 0x0);
}

void irq_install()
{
    irq_remap();

    idt_set_gate(32, (unsigned)irq0, 0x08, 0x8E);
    idt_set_gate(33, (unsigned)irq1, 0x08, 0x8E);
    idt_set_gate(34, (unsigned)irq2, 0x08, 0x8E);
    idt_set_gate(35, (unsigned)irq3, 0x08, 0x8E);
    idt_set_gate(36, (unsigned)irq4, 0x08, 0x8E);
    idt_set_gate(37, (unsigned)irq5, 0x08, 0x8E);
    idt_set_gate(38, (unsigned)irq6, 0x08, 0x8E);
    idt_set_gate(39, (unsigned)irq7, 0x08, 0x8E);
    idt_set_gate(40, (unsigned)irq8, 0x08, 0x8E);
    idt_set_gate(41, (unsigned)irq9, 0x08, 0x8E);
    idt_set_gate(42, (unsigned)irq10, 0x08, 0x8E);
    idt_set_gate(43, (unsigned)irq11, 0x08, 0x8E);
    idt_set_gate(44, (unsigned)irq12, 0x08, 0x8E);
    idt_set_gate(45, (unsigned)irq13, 0x08, 0x8E);
    idt_set_gate(46, (unsigned)irq14, 0x08, 0x8E);
    idt_set_gate(47, (unsigned)irq15, 0x08, 0x8E);
}

/* Each of the IRQ ISRs point to this function.
*  The IRQ Controllers need
*  to be told when you are done servicing them, so you need
*  to send them an "End of Interrupt" command (0x20). There
*  are two 8259 chips: The first exists at 0x20, the second
*  exists at 0xA0. If the second controller (an IRQ from 8 to
*  15) gets an interrupt, you need to acknowledge the
*  interrupt at BOTH controllers, otherwise, you only send
*  an EOI command to the first controller. If you don't send
*  an EOI, you won't raise any more IRQs */
void irq_handler(struct regs *r)
{
    /* blank function pointer */
    void (*handler)(struct regs *r);

    /* Search for custom handler to run for this
    *  IRQ, run it */
    handler = irq_routines[r->int_no - 32];
    if (handler)
    {
        handler(r);
    }

    /* If the IDT entry that was invoked was greater than 40
    *  (meaning IRQ8 - 15), then we need to send an EOI to
    *  the slave controller */
    if (r->int_no >= 40)
    {
        outportb(0xA0, 0x20);
    }

    /* In either case, we need to send an EOI to the master
    *  interrupt controller too */
    outportb(0x20, 0x20);
}

/* ======== Programmable Interval Timer (PIT) ======== */
/*
 * Programmable Interval Timer
 * -----------------------
 * |7   6 5  4 3     1  0|
 * -----------------------
 * |CNTR  RW   Mode  BCD |
 * -----------------------
 * CNTR = Counter num (0-2)
 * RW =  Read Write Mode
 * Mode =
 *   0     Interrupt on terminal count
 *   1     Hardware retriggerable one shot
 *   2     Rate generator
 *   3     square wave  
 *   4     software strobe 
 *   5     hardware strobe 
 *
 * BCD = binary coded decimal
 *   0 = 16 bit counter
 *   1 = 4x BCD decay counter
 */
void timer_phase(int hz)
{
    int divisor = 1193180 / hz;       /* Calculate our divisor */
    outportb(0x43, 0x36);             /* Set our command byte 0x36 */
    outportb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    outportb(0x40, divisor >> 8);     /* Set high byte of divisor */
}

unsigned int timer_ticks = 0;

void timer_wait(int ticks)
{
    unsigned int eticks;
 
    eticks = timer_ticks + ticks;
    while(timer_ticks < eticks) 
    {
        __asm__ __volatile__ ("sti//hlt//cli");
    }
}

/* Increment ticks every time the timer fires */
void timer_handler(struct regs *r)
{
    /* Increment our 'tick count' */
    timer_ticks++;

    /* every 100 clocks, display msg */
    if (timer_ticks % 100 == 0)
    {
        puts("Timer: log \n");
    }
}

/* Sets up the system clock by installing the timer handler
*  into IRQ0 */
/* 
 * Set sys clock by installing timer handle to IRQ0
 * 
 * set timer phase at 100 hz
*/
void timer_install()
{
    timer_phase(SYS_FREQ);

    /* Installs 'timer_handler' to IRQ0 */
    irq_install_handler(0, timer_handler);
}


/* ======== Keyboard ======== */
/* US Keyboard Layout */
unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};		

/* Handles the keyboard interrupt */
void keyboard_handler(struct regs *r)
{
    unsigned char scancode;

    /* Read from kb data buffer */
    scancode = inportb(0x60);

    /* If the top bit of the byte we read from kb  is
    *  set, that means that a key has just been released */
    if (scancode & 0x80)
    {
        // TODO handle shift, alt, ctrl
    }
    else
    {
        /* Handle key presses. If held , multiple interrupts will be issued */
        puts("LOG");
    }
}

/* Install keyboard handler into IRQ1 */
void keyboard_install()
{
    irq_install_handler(1, keyboard_handler);
}

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

