#ifndef _KERNEL_IDT_H
#define _KERNEL_IDT_H

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


/* lives in boot.S */
extern void idt_load();

void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);

void idt_install();

#endif
