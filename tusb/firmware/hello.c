#include <8051.h>
#include "TUSB3210.h"

#define LED P3_0

void reset_watchdog() {
   WDCSR |= WDT;
}

void delay(int i) {
   int j;
   for (j=0;j<i;j++)
      reset_watchdog();
}

void main() {
   while (1) {
      LED = 0;
      delay(1000);
      LED = 1;
      delay(20000);
   }
}

