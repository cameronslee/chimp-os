#include <stdio.h>
#include <string.h>
#include <security.h>

#include <kernel/tty.h>

#define NUM_COLORS 16

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

/* ======== Kernel ======== */
void kernel_main(void) {
    gdt_install();
    idt_install();
    isrs_install();
    terminal_initialize();
    const char* d = "                               Welcome to Chimp OS\n";
    terminal_writestring((const char *) d);
    //asm volatile ("1: jmp 1b"); // pseudo breakpoint

    //This should trigger a division by zero isr
    __asm__  ("div %0" :: "r"(0));
}

