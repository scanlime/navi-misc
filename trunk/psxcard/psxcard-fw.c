/*
 * psxcard.c - Firmware for a USB Playstation card reader, implemented
 *             on a TUSB3410 microcontroller (Modified Keyspan USA19HS)
 *
 * Hardware:
 *
 *     All inputs and outputs routed through CMOS inverter. 4.7k
 *     pull-ups on inputs from memory card, 9.2k pull-ups on inputs
 *     from TUSB3410. 330 ohm current limiting resistor in series
 *     with TUSB3410 inputs / inverter outputs.
 *
 *     Memory card and inverter running off the same power supply,
 *     ~3.6v created by stringing a couple diodes off the 5V supply.
 *
 *     Signal mapping: (Note that all signals are inverted)
 *
 *     P3_0 -> CLK
 *     P3_1 <- DAT
 *     DTR  -> CMD
 *     RTS  -> SEL
 *     CTS  <- ACK
 *
 *     NB: Using CTS for ACK seems logical, since we have hardware
 *         change detection on the CTS and DSR pins. It would make
 *         more sense to use P3_2 and P3_3 for the other two signals,
 *         but those two GPIOs are problematic to access on the
 *         Keyspan board.
 *
 * --Micah Dowty <micah@navi.cx>
 */

#include <stdio.h>
#include <string.h>
#include "usb_driver.h"
#include "uart_driver.h"
#include "util.h"

#define ARRAYSIZE(a) ((sizeof(a)/sizeof((a)[0])))

/*
 * Hardware-specific definitions
 */
#define CLK_HIGH() P3_0 = 0
#define CLK_LOW()  P3_0 = 1
#define CMD_HIGH() MCR |= MCR_DTR
#define CMD_LOW()  MCR &= ~MCR_DTR
#define SEL_HIGH() MCR |= MCR_RTS
#define SEL_LOW()  MCR &= ~MCR_RTS
#define DAT_TEST() (!P3_1)
#define ACK_TEST() (MSR & MSR_LCTS)

/* We give SDCC from 0xF800 to 0xFCFF as general-purpose xdata,
 * the rest is available for buffers. Start at 0xFD00. Note that
 * DMA buffers must be aligned on an 8-byte boundary.
 */
volatile xdata at 0xFD00 unsigned char ep1_out_x[64];
volatile xdata at 0xFD40 unsigned char ep1_in_x[64];


/********************************************************************/
/************************************************* PSX Card Slot ****/
/********************************************************************/

void cardslot_init()
{
    /* Tristate all GPIOs */
    P3_0 = 1;
    P3_1 = 1;
    P3_2 = 1;
    P3_3 = 1;

    /* Outputs to their default states */
    CLK_HIGH();
    CMD_HIGH();
    SEL_HIGH();
}

/*
 * The Playstation bus is full-duplex, and we're always reading and
 * writing data simultaneously.  Often only the read data or only the
 * written data are significant, but sometimes both are. This function
 * writes and reads a single byte. Does not wait for ACK.
 *
 * For speed, this loop has been unrolled. The typical clock rate for
 * PSX cards is 250 KHz. This microcontroller is slow enough that we
 * shouldn't have to worry about exceeding that overall rate, but we
 * should be careful that the width of the clk low pulse isn't too
 * short. That's where we choose to reset the watchdog timer.
 */
unsigned char cardslot_txrx_byte(unsigned char outByte)
{
    unsigned char inByte = 0;

#define TXRX_BIT(mask) \
    if (outByte & mask) { \
        CMD_HIGH(); \
    } else { \
        CMD_LOW(); \
    } \
    CLK_LOW(); \
    watchdog_reset(); \
    CLK_HIGH(); \
    if (DAT_TEST()) { \
       inByte |= mask; \
    }

    TXRX_BIT(0x01)
    TXRX_BIT(0x02)
    TXRX_BIT(0x04)
    TXRX_BIT(0x08)
    TXRX_BIT(0x10)
    TXRX_BIT(0x20)
    TXRX_BIT(0x40)
    TXRX_BIT(0x80)

#undef TXRX_BIT
    return inByte;
}


/********************************************************************/
/************************************************* USB Interface ****/
/********************************************************************/


void main() {
  uart_init();
  puts("\n---- Startup ----");
  cardslot_init();
  puts("Card slot initialized");
  usb_init();
  puts("USB initialized");

  /* Prepare our bulk endpoints, set up the first EP1 OUT transfer */
  usb_dma_unstall(EDB_IEP1);
  usb_dma_unstall(EDB_OEP1);
  usb_dma_setup(EDB_OEP1, ep1_out_x, sizeof(ep1_out_x));

  /* XXX: Send an initial EP1 IN, but with bogus data */
  usb_dma_setup(EDB_IEP1, ep1_in_x, sizeof(ep1_in_x));

  while (1) {
    int c;

    watchdog_reset();
    usb_poll();

    c = usb_dma_status(EDB_IEP1);
    if (c) {
      unsigned char buffer[] = {
	0x01, 0x42, 0x00,
	0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
      };
      unsigned char i;

      SEL_LOW();
      for (i=0; i<ARRAYSIZE(buffer); i++) {
	ep1_in_x[i] = cardslot_txrx_byte(buffer[i]);
      }
      SEL_HIGH();

      usb_dma_setup(EDB_IEP1, ep1_in_x, i);
    }


    /* Poll for completed DMA transfers from the EP1 OUT endpoint */
    c = usb_dma_status(EDB_OEP1);
    if (c) {
      printf("Received %d bytes on EP1 OUT: %02X %02X %02X %02X...\n", c,
	     ep1_out_x[0], ep1_out_x[1], ep1_out_x[2], ep1_out_x[3]);
      usb_dma_setup(EDB_OEP1, ep1_out_x, sizeof(ep1_out_x));
    }
  }
}


void usb_handle_vendor_request() {
  printf("Vendor request %d\n", usb_setup_buffer.bRequest);

  switch (usb_setup_buffer.bRequest) {

  case 0:
    usb_write_ack();
    break;

  }
}

/* The End */
