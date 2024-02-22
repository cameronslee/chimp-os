#include <stdio.h>
#include <string.h>
#include <security.h>

#include <kernel/tty.h>

#define NUM_COLORS 16

/* ======== IDT ======== */

/* Defines an IDT entry */
struct idt_entry
{
    unsigned short base_lo;
    unsigned short sel;        /* Our kernel segment goes here! */
    unsigned char always0;     /* This will ALWAYS be set to 0! */
    unsigned char flags;       /* Set using the above table! */
    unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

/* Declare an IDT of 256 entries. Although we will only use the
*  first 32 entries in this tutorial, the rest exists as a bit
*  of a trap. If any undefined IDT entry is hit, it normally
*  will cause an "Unhandled Interrupt" exception. Any descriptor
*  for which the 'presence' bit is cleared (0) will generate an
*  "Unhandled Interrupt" exception */
struct idt_entry idt[256];
struct idt_ptr idtp;

/* asm routine in boot.S */
extern void idt_load();

void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags) 
{
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = ((base >> 16) & 0xFFFF);

    idt[num].sel = sel;
    idt[num].flags = flags;

    idt[num].always0 = 0;
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
    *  Please check the table above in the tutorial in order
    *  to see exactly what each value means */
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
    int foobar = 5 / 0;
    terminal_writestring(d);
    // This should cause a system reset. need to handle with ISRs
    terminal_writestring((const char *)foobar);
}

