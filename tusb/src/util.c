#include <tusb.h>

void watchdog_reset() {
  WDCSR |= WDT;
}

void delay(unsigned int i) {
  unsigned int j;
  for (j=0;j<i;j++)
    watchdog_reset();
}
