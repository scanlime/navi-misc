#include <tusb.h>
#include <stdio.h>
#include "usb_driver.h"
#include "uart_driver.h"
#include "util.h"

void main() {
  EA = 0;     /* Global interrupt disable */

  uart_init();
  puts("\n---- Startup ----");

  usb_init();
  puts("USB initialized");

  while (1) {
    usb_poll();
    delay(20000);
  }
}

