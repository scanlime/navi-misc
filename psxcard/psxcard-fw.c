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
 * USB Protocol:
 *     See psxcard-proto.h
 *
 * --Micah Dowty <micah@navi.cx>
 */

#include <stdio.h>
#include <string.h>
#include <usb_driver.h>
#include <uart_driver.h>
#include <util.h>
#include "psxcard-proto.h"

#define ARRAYSIZE(a) ((sizeof(a)/sizeof((a)[0])))
#define MIN(a, b)    ((a) < (b) ? (a) : (b))
#define TRUE         1
#define FALSE        0
typedef unsigned char uint8;
typedef unsigned short uint16;

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
#define ACK_CHANGE_RESET() MSR = MSR_DCTS
#define ACK_CHANGE_TEST()  (MSR & MSR_DCTS)

/*
 * We give SDCC from 0xF800 to 0xFCFF as general-purpose xdata,
 * the rest is available for buffers. Start at 0xFD00. Note that
 * DMA buffers must be aligned on an 8-byte boundary.
 *
 * XXX: We don't yet double-buffer, but that could increase
 *      speed quite a bit since this is so CPU-intensive.
 */
volatile xdata at 0xFD00 uint8 ep1_out_x[64];
volatile xdata at 0xFD40 uint8 ep1_in_x[64];

PsxcardErrors errors;

xdata struct {
    uint8 read[PSXCARD_FRAME_SIZE];
    uint8 write[PSXCARD_FRAME_SIZE];

    /* Amount of data currently buffered. Up to PSXCARD_FRAME_SIZE. */
    uint8 read_offset;
    uint8 write_size;

    /* Frame number of data currently in the buffer. */
    uint16 read_frame;
    uint16 write_frame;

    /* Nonzero if a read/write is currently in progress. */
    uint8 read_active;
    uint8 write_active;
} buffers;


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

void cardslot_begin()
{
    SEL_LOW();
    ACK_CHANGE_RESET();
}

void cardslot_end()
{
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
uint8 cardslot_txrx_byte(uint8 outByte)
{
    uint8 inByte = 0;

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

/*
 * Wait for an ACK. Returns TRUE if an ACK was received, FALSE if not.
 * This operates using the UART's change-detection circuitry.
 */
uint8 cardslot_wait_for_ack(void)
{
    uint16 i;
    for (i=0; i<1000; i++) {
	if (ACK_CHANGE_TEST()) {
	    ACK_CHANGE_RESET();
	    return TRUE;
	}
    }
    return FALSE;
}

/*
 * Read one frame from a Playstation memory card. Returns TRUE
 * if the read was successful, FALSE on any error.
 */
uint8 cardslot_read_frame(uint16 frame, xdata uint8 *buffer)
{
    uint8 i;
    uint8 frameHigh = frame >> 8;
    uint8 frameLow = (uint8) frame;
    uint8 checkbyte = frameHigh ^ frameLow;

    cardslot_begin();

    /* Select the memory card, slot 1 */
    cardslot_txrx_byte(0x81);
    if (!cardslot_wait_for_ack()) goto error;

    /* Memory card read */
    cardslot_txrx_byte(0x52);
    if (!cardslot_wait_for_ack()) goto error;

    /* Some magic 2-byte response... */
    if (cardslot_txrx_byte(0x00) != 0x5a) goto error;
    if (!cardslot_wait_for_ack()) goto error;
    if (cardslot_txrx_byte(0x00) != 0x5d) goto error;
    if (!cardslot_wait_for_ack()) goto error;

    /* Send address */
    cardslot_txrx_byte(frameHigh);
    if (!cardslot_wait_for_ack()) goto error;
    cardslot_txrx_byte(frameLow);
    if (!cardslot_wait_for_ack()) goto error;

    /* Magic 1-byte acknowledgment */
    if (cardslot_txrx_byte(0x00) != 0x5c) goto error;
    if (!cardslot_wait_for_ack()) goto error;
    
    /* Wait for 'start of data' byte */
    i = 100;
    while (cardslot_txrx_byte(0x00) != 0x5d) {
	if (!cardslot_wait_for_ack()) goto error;
	if (!--i) goto error;
    }
    if (!cardslot_wait_for_ack()) goto error;

    /* Read a confirmation copy of the frame address */
    if (cardslot_txrx_byte(0x00) != frameHigh) goto error;
    if (!cardslot_wait_for_ack()) goto error;
    if (cardslot_txrx_byte(0x00) != frameLow) goto error;
    if (!cardslot_wait_for_ack()) goto error;

    /*
     * Finally read the data itself. Along the way,
     * update our copy of the XOR check byte.
     */
    for (i=0; i<PSXCARD_FRAME_SIZE; i++) {
	checkbyte ^= buffer[i] = cardslot_txrx_byte(0x00);
	if (!cardslot_wait_for_ack()) goto error;
    }

    /* Read and verify the check byte */
    if (cardslot_txrx_byte(0x00) != checkbyte) goto error;
    if (!cardslot_wait_for_ack()) goto error;
    
    /* The last byte is an end-of-data flag, and isn't ACK'ed */
    if (cardslot_txrx_byte(0x00) != 0x47) goto error;

    cardslot_end();
    return TRUE;

 error:
    cardslot_end();
    return FALSE;
}


/********************************************************************/
/************************************************* USB Interface ****/
/********************************************************************/

void usbreq_count_error(uint8 *ptr)
{
    if (*ptr != 0xFF) {
	(*ptr)++;
    }
}   

/* Reset the state associated with all USB read/write requests */
void usbreq_init()
{
    memset(&errors, 0, sizeof errors);
    memset(&buffers, 0, sizeof buffers);
}

/*
 * Begin processing a read on the specified frame. This immediately
 * loads this frame from the card, and updates error counters as
 * necessary.
 */
void usbreq_read(uint16 frame)
{
    uint8 retries = 10;
    uint8 blockSize;

    /*
     * If the frame is now out of range, end the read
     * without triggering an error.
     */
    if (frame >= 1024) {
	buffers.read_active = 0;
	return;
    }

    buffers.read_frame = frame;

    while (!cardslot_read_frame(frame, buffers.read)) {
	usbreq_count_error(&errors.soft);
	if (!--retries) {
	    usbreq_count_error(&errors.hard);
	    buffers.read_active = 0;
	    return;
	}
    }
    
    blockSize = MIN(sizeof ep1_out_x, PSXCARD_FRAME_SIZE);
    memcpy(ep1_out_x, buffers.read, blockSize);
    usb_dma_unstall(EDB_IEP1);
    usb_dma_setup(EDB_IEP1, ep1_out_x, blockSize);

    buffers.read_offset = blockSize;
    buffers.read_active = 1;
}


/*
 * Prepare for the next sequential block in a read
 */
void usbreq_read_next(void)
{
    uint8 blockSize;

    if (buffers.read_offset == PSXCARD_FRAME_SIZE) {
	usbreq_read(buffers.read_frame + 1);
	return;
    }

    blockSize = MIN(sizeof ep1_out_x, PSXCARD_FRAME_SIZE - buffers.read_offset);
    memcpy(ep1_out_x, buffers.read + buffers.read_offset, blockSize);
    usb_dma_setup(EDB_IEP1, ep1_out_x, blockSize);
    buffers.read_offset += blockSize;
}

void usbreq_write(uint16 frame)
{
}

void main()
{
  uart_init();
  puts("\n---- Startup ----");
  cardslot_init();
  puts("Card slot initialized");
  usb_init();
  puts("USB initialized");

  /* Simulate an initial INIT request, to put us in a known state */
  usbreq_init();

  while (1) {
    watchdog_reset();
    usb_poll();

    if (buffers.read_active && usb_dma_status(EDB_IEP1)) {
	usbreq_read_next();
    }
  }
}


void usb_handle_vendor_request() {
  switch (usb_setup_buffer.bRequest) {

  case PSXCARD_REQ_INIT:
      usbreq_init();
      usb_write_ack();
      break;

  case PSXCARD_REQ_ERR_CHECK:
      usb_write_ep0_buffer(&errors, sizeof errors);
      memset(&errors, 0, sizeof errors);
      break;

  case PSXCARD_REQ_READ:
      usbreq_read(usb_setup_buffer.wValue);
      usb_write_ack();
      break;

  case PSXCARD_REQ_WRITE:
      usbreq_write(usb_setup_buffer.wValue);
      usb_write_ack();
      break;

  }
}

/* The End */
