#include <tusb.h>
#include <stdio.h>
#include <string.h>
#include "usb_driver.h"
#include "uart_driver.h"
#include "util.h"

struct status {
  unsigned char out_config, out_count, in_config, in_count;
};

void show_differences() {
  struct status current;
  static struct status old;
  current.out_config = OEPCNFG_0;
  current.out_count = OEPBCNT_0;
  current.in_config = IEPCNFG_0;
  current.in_count = IEPBCNT_0;
  if (memcmp(&current, &old, sizeof(current))) {
    usb_ep0out_buffer[7] = 0;
    printf(" Changed:  IN[ cfg 0x%02X, count 0x%02X ]  OUT[ cfg 0x%02X, count 0x%02X, buffer '%s']\n",
	   current.in_config, current.in_count, current.out_config, current.out_count, usb_ep0out_buffer);
    memcpy(&old, &current, sizeof(current));
  }
}

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
    show_differences();
  }
}


void usb_handle_vendor_request() {
  static unsigned char v = 0;
  v++;

  printf("Vendor request %d\n", usb_setup_buffer.bRequest);

  switch (usb_setup_buffer.bRequest) {

  case 42:
    /* ACK the setup packet */
    OEPBCNT_0 = 0;

    /* Leave setup */
    USBCTL &= ~SIR;
    USBSTA = SETUP;

    /* Wait for our OUT data to arrive */
    printf("Waiting for OUT data\n");
    while ((OEPBCNT_0 & 0x80) == 0)
      watchdog_reset();
    printf("Received data: '%s'\n", usb_ep0out_buffer);
    break;

  case 123:
    printf("Sending %d\n", v);
    usb_write_ep0_buffer(&v, 1);
    break;

  }
}
