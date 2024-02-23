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
    /* We'll leave you to try and code this function: take the
    *  argument 'base' and split it up into a high and low 16-bits,
    *  storing them in idt[num].base_hi and base_lo. The rest of the
    *  fields that you must set in idt[num] are fairly self-
    *  explanatory when it comes to setup */
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;

    idt[num].always0 = 0;
    idt[num].flags = flags;
    idt[num].sel = sel;
}

/* Installs the IDT */
void idt_install()
{
    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = (unsigned int) &idt;

    memset(&idt, 0, sizeof(struct idt_entry) * 256);

    // TODO add new ISRS to IDT here

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

/* Function lives in boot.S */
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
		

/* ======== Kernel ======== */
void kernel_main(void) {
    gdt_install();
    idt_install();
    terminal_initialize();
    const char* d = "                               Welcome to Chimp OS\n";
    terminal_writestring(d);
    /*
    This should cause a system reset, need to handle with ISR
    int foo = 5 / 0;
    terminal_writestring((const char *) foo);
    */
}

