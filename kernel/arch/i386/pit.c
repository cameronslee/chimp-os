#include <stdio.h>
#include <kernel/system.h>
#include <kernel/pit.h>
#include <kernel/irq.h>

#define SYS_FREQ 100

unsigned int timer_ticks = 0;

void timer_phase(int hz)
{
    int divisor = 1193180 / hz;       /* Calculate our divisor */
    outportb(0x43, 0x36);             /* Set our command byte 0x36 */
    outportb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    outportb(0x40, divisor >> 8);     /* Set high byte of divisor */
}

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
        puts("======");
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

