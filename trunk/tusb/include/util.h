#ifndef __UTIL_H
#define __UTIL_H

#include <tusb.h>

/* GPIO allocation on the Keyspan USA-19HS board */
#define LED          P3_0
#define RS232_POWER  P3_1

void watchdog_reset();

/* Delay, in units of approximately 10us */
void delay(unsigned int i);

#endif /* __UTIL_H */
