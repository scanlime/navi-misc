#include <tusb.h>
#include <stdio.h>
#include "usb_driver.h"
#include "util.h"

void usb_init() {
  FUNADR = 0;       /* We haven't been assigned an address yet */

  /* Set up endpoint 0 */
  IEPCNFG_0 = STALL | USBIE | UBME;
  OEPCNFG_0 = STALL | USBIE | UBME;

  USBCTL &= ~CONT;  /* Disconnect from USB */
  USBSTA = 0xFF;    /* Clear USB status bits */
  delay(60000);     /* Wait a bit and reconnect */
  USBCTL |= CONT;
}

void usb_handle_setup() {
  USBCTL |= SIR;    /* Indicate that we're busy with a setup packet */

  /* Set the direction bit */
  if (usb_setup_buffer.request_type & USB_DIR_IN)
    USBCTL |= DIR;
  else
    USBCTL &= ~DIR;

  printf("   request_type=%d request=%d value=%d index=%d length=%d\n",
	 usb_setup_buffer.request_type,
	 usb_setup_buffer.request,
	 usb_setup_buffer.value,
	 usb_setup_buffer.index,
	 usb_setup_buffer.length);
}

void usb_poll() {
  unsigned char status;
  status = USBSTA;     /* Read all status bits and clear them all */
  USBSTA = 0xFF;

  if (status & RSTR) {
    puts("Function reset request");
  }

  if (status & SUSR) {
    puts("Function suspend request");
  }

  if (status & RESR) {
    puts("Function resume request");
  }

  if (status & STPOW) {
    puts("*** SETUP overwrite");
  }

  if (status & SETUP) {
    puts("SETUP received");
    usb_handle_setup();
  }
}
