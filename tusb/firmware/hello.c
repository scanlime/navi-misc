#include <tusb.h>
#include <stdio.h>
#include "usb_driver.h"
#include "uart_driver.h"
#include "util.h"

void main() {
  EA = 0;     /* Global interrupt disable */

  uart_init();
  puts("\n---- Startup ----");

#if 0
  usb_init();
  puts("USB initialized");
#else
  printf("Not reinitializing USB, using address %d\n", FUNADR);
#endif

  while (1) {
    watchdog_reset();
    usb_poll();
  }
}

