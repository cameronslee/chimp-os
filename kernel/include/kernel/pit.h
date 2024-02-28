#ifndef _KERNEL_PIT_H
#define _KERNEL_PIT_H

#include <kernel/system.h>

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

void timer_phase(int hz);

void timer_wait(int ticks);

void timer_handler(struct regs *r);

void timer_install();

#endif
