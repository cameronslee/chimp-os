#include <kernel/idt.h>
#include <string.h>

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
