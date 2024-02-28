#ifndef _KERNEL_GDT_H
#define _KERNEL_GDT_H

/* ======== GDT ======== */
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

/* asm routine lives in boot.S */
extern void gdt_flush();

void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);

void gdt_install();

#endif
