#ifndef _KERNEL_SYSTEM_H
#define _KERNEL_SYSTEM_H

// Typedefs written for 32-bit X86. For standardization across platforms
typedef unsigned int   u32int;
typedef          int   s32int;
typedef unsigned short u16int;
typedef          short s16int;
typedef unsigned char  u8int;
typedef          char  s8int;

/* Stack architecture once ISR runs */
struct regs
{
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'push' */
    unsigned int int_no, err_code;    /* push #' and err code*/
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};

unsigned char inportb (unsigned short _port);

void outportb(unsigned short port, unsigned char value);

char* itoa(int value, char *str, int base); // TODO move to stdlib

#endif
