#include <kernel/system.h>

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
