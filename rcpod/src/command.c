/*
 * command.c - General purpose I/O commands implemented for the rcpod
 *
 * Remote Controlled PIC of Doom
 * Copyright (C) 2003 Micah Dowty <micah@picogui.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <string.h>
#include <errno.h>
#include <rcpod_private.h>


/*************************************************************************************/
/************************************************** Low-level commands ***************/
/*************************************************************************************/

/*
 * The commands in this section map directly to requests in the rcpod protocol
 */

void rcpod_Poke(rcpod_dev* rcpod, int address, unsigned char data) {
  int retval;
  /* The address and data are sent in the wIndex and wValue parameters, respectively */
  retval = usb_control_msg(rcpod->usbdevh, USB_TYPE_VENDOR, RCPOD_CTRL_POKE,
			   data, address, NULL, 0, RCPOD_TIMEOUT);
  if (retval < 0)
    rcpod_HandleError("rcpod_Poke", errno, strerror(errno));
}


unsigned char rcpod_Peek(rcpod_dev* rcpod, int address) {
  int retval;
  unsigned char byte;
  /* Send the address in wIndex, expect a 1-byte response packet with the data */
  retval = usb_control_msg(rcpod->usbdevh, USB_TYPE_VENDOR | USB_ENDPOINT_IN,
			   RCPOD_CTRL_PEEK, 0, address, (char*) &byte, 1, RCPOD_TIMEOUT);
  if (retval < 0) {
    rcpod_HandleError("rcpod_Peek", errno, strerror(errno));
    return 0;
  }
  return byte;
}


void rcpod_Poke4(rcpod_dev* rcpod, unsigned char data[4]) {
  int retval;
  /* All pin descriptors are packed into the four bytes of wValue and wIndex such
   * that in the control message header the bytes are contiguous.
   */
  retval = usb_control_msg(rcpod->usbdevh, USB_TYPE_VENDOR, RCPOD_CTRL_POKE4,
			   data[0] | (((int)data[1])<<8),
			   data[2] | (((int)data[3])<<8),
			   NULL, 0, RCPOD_TIMEOUT);
  if (retval < 0)
    rcpod_HandleError("rcpod_Poke4", errno, strerror(errno));
}


void rcpod_Peek8(rcpod_dev* rcpod, int address, unsigned char data[8]) {
  int retval;
  retval = usb_control_msg(rcpod->usbdevh, USB_TYPE_VENDOR | USB_ENDPOINT_IN,
			   RCPOD_CTRL_PEEK8, 0, address, (char*) data, 8, RCPOD_TIMEOUT);
  if (retval < 0)
    rcpod_HandleError("rcpod_Peek8", errno, strerror(errno));
}


void rcpod_AnalogReadAll(rcpod_dev* rcpod, unsigned char buffer[8]) {
  int retval;
  retval = usb_control_msg(rcpod->usbdevh, USB_TYPE_VENDOR | USB_ENDPOINT_IN,
			   RCPOD_CTRL_ANALOG_ALL, 0, 0, (char*) buffer, 8, RCPOD_TIMEOUT);
  if (retval < 0)
    rcpod_HandleError("rcpod_AnalogReadAll", errno, strerror(errno));
}


void rcpod_UsartTxRx(rcpod_dev* rcpod, int address, int txBytes, int rxBytes) {
  int retval;
  retval = usb_control_msg(rcpod->usbdevh, USB_TYPE_VENDOR, RCPOD_CTRL_USART_TXRX,
			   txBytes | (((int)rxBytes) << 8), address, NULL, 0, RCPOD_TIMEOUT);
  if (retval < 0)
    rcpod_HandleError("rcpod_UsartTxRx", errno, strerror(errno));

  /* Zero our our receive tracking info, since this has cancelled any receive in progress */
  memset(&rcpod->rx, 0, sizeof(rcpod->rx));

  /* Prepare our rx structure for tracking this receive, if we just started one */
  if (rxBytes) {
    rcpod->rx.address = address;
    rcpod->rx.size = rxBytes;
    /* Zero is the proper default for all other fields */
  }
}


int rcpod_UsartRxProgress(rcpod_dev* rcpod) {
  int retval;
  unsigned char byteCount;
  retval = usb_control_msg(rcpod->usbdevh, USB_TYPE_VENDOR | USB_ENDPOINT_IN,
			   RCPOD_CTRL_USART_RX_PROGRESS, 0, 0,
			   (char*) &byteCount, 1, RCPOD_TIMEOUT);
  if (retval < 0) {
    rcpod_HandleError("rcpod_UsartRxProgress", errno, strerror(errno));
    return 0;
  }
  return byteCount;
}


void rcpod_UsartTxe(rcpod_dev* rcpod, rcpod_pin txe) {
  int retval;
  retval = usb_control_msg(rcpod->usbdevh, USB_TYPE_VENDOR, RCPOD_CTRL_USART_TXE,
			   txe, 0, NULL, 0, RCPOD_TIMEOUT);
  if (retval < 0)
    rcpod_HandleError("rcpod_UsartTxe", errno, strerror(errno));
}


void rcpod_GpioAssert4(rcpod_dev* rcpod, rcpod_pin pins[4]) {
  int retval;
  /* All pin descriptors are packed into the four bytes of wValue and wIndex such
   * that in the control message header the bytes are contiguous.
   */
  retval = usb_control_msg(rcpod->usbdevh, USB_TYPE_VENDOR, RCPOD_CTRL_GPIO_ASSERT,
			   pins[0] | (((int)pins[1])<<8),
			   pins[2] | (((int)pins[3])<<8),
			   NULL, 0, RCPOD_TIMEOUT);
  if (retval < 0)
    rcpod_HandleError("rcpod_GpioAssert4", errno, strerror(errno));
}


int rcpod_GpioTest(rcpod_dev* rcpod, rcpod_pin pin) {
  int retval;
  unsigned char byte;
  /* Send the address in wIndex, expect a 1-byte response packet with the data */
  retval = usb_control_msg(rcpod->usbdevh, USB_TYPE_VENDOR | USB_ENDPOINT_IN,
			   RCPOD_CTRL_GPIO_READ, 0, pin, (char*) &byte, 1, RCPOD_TIMEOUT);
  if (retval < 0) {
    rcpod_HandleError("rcpod_GpioTest", errno, strerror(errno));
    return 0;
  }
  return byte;
}


/*************************************************************************************/
/************************************************** Medium-level commands ************/
/*************************************************************************************/

/*
 * These are convenience functions, implemented using the protocol
 * requests encapsulated by the low-level commands.
 */

void rcpod_PeekBuffer(rcpod_dev* rcpod, int address, unsigned char *buffer, int bytes) {
  while (bytes > 0) {
    if (bytes >= 8) {
      /* We can use the more efficient peek8 command */
      rcpod_Peek8(rcpod, address, buffer);
      buffer += 8;
      address += 8;
      bytes -= 8;
    }
    else {
      /* Get the rest in smaller chunks. In most cases it would be
       * fine to use peek8 again to get the rest and ignore the parts
       * we don't need, but since reading some registers can have side effects
       * this isn't safe.
       */
      *buffer = rcpod_Peek(rcpod, address);
      buffer++;
      address++;
      bytes--;
    }
  }
}


void rcpod_PokeBuffer(rcpod_dev* rcpod, int address, unsigned char *buffer, int bytes) {
  /* The first byte must be sent using poke, since poke4 picks
   * an address following the previous poke command.
   */
  if (bytes > 0) {
    rcpod_Poke(rcpod, address, *buffer);
    address++;
    buffer++;
    bytes--;
  }

  while (bytes > 0) {
    if (bytes >= 4) {
      /* We can use the more efficient poke4 command */
      rcpod_Poke4(rcpod, buffer);
      address += 4;
      buffer += 4;
      bytes -= 4;
    }
    else {
      rcpod_Poke(rcpod, address, *buffer);
      address++;
      buffer++;
      bytes--;
    }
  }
}


void rcpod_GpioAssert(rcpod_dev* rcpod, rcpod_pin pin) {
  /* This just calls GpioAssert4 with the last three pin descriptors no-op'ed */
  rcpod_pin pins[] = {0,0,0,0};
  pins[0] = pin;
  rcpod_GpioAssert4(rcpod, pins);
}


void rcpod_GpioAssertBuffer(rcpod_dev* rcpod, rcpod_pin *pins, int count) {
  rcpod_pin pins4[] = {0,0,0,0};

  /* First send as many blocks of 4 as we can */
  while (count >= 4) {
    rcpod_GpioAssert4(rcpod, pins);
    count -= 4;
    pins += 4;
  }

  /* Copy the remaining pins into the zero-padded buffer of 4, and send that */
  if (count > 0) {
    memcpy(pins4, pins, count * sizeof(rcpod_pin));
    rcpod_GpioAssert4(rcpod, pins4);
  }
}


void rcpod_GpioDeassert(rcpod_dev* rcpod, rcpod_pin pin) {
  rcpod_GpioAssert(rcpod, RCPOD_NEGATE(pin));
}


void rcpod_GpioDeassertBuffer(rcpod_dev* rcpod, rcpod_pin *pins, int count) {
  rcpod_pin pins4[] = {0,0,0,0};
  int blockSize, i;

  /* Negate up to four pin descriptors at a time and send them with GpioAssertBuffer */
  while (count > 0) {
    blockSize = count > 4 ? 4 : count;
    for (i=0; i<blockSize; i++)
      pins4[i] = RCPOD_NEGATE(pins[i]);
    rcpod_GpioAssertBuffer(rcpod, pins4, blockSize);
    pins += blockSize;
    count -= blockSize;
  }
}


unsigned char rcpod_AnalogReadChannel(rcpod_dev* rcpod, int channel) {
  /* Turn on the A/D converter and set it to the proper channel */
  rcpod_Poke(rcpod, RCPOD_REG_ADCON0, 0x81 | (channel << 3));

  /* Normally we'd perform an aquisition delay now to let the holding capacitor
   * charge, but because the USB interface is so slow we can ignore this.
   */

  /* Start the A/D conversion */
  rcpod_Poke(rcpod, RCPOD_REG_ADCON0, 0x85 | (channel << 3));

  /* Normally we'd also have to wait for the conversion to finish, but
   * the USB interface is slow enough the result should already be ready
   */
  return rcpod_Peek(rcpod, RCPOD_REG_ADRES);
}


/*************************************************************************************/
/************************************************** Serial I/O ***********************/
/*************************************************************************************/

void rcpod_SerialInit(rcpod_dev* rcpod, int baudRate) {
  int spbrg;
  int brgh;

  /* Calculate the value of the SPBRG register for BRGH=1 and SYNC=0:
   *    baud = Fint / (16 * (SPBRG+1))
   *    Fint = 24000000
   *    16 * (SPBRG+1) = Fint / baud
   *    SPBRG = Fint / (baud * 16) - 1
   */
  spbrg = 24000000 / (baudRate * 16) - 1;
  brgh = 0x04;

  /* SPBRG is an 8-bit register, if our baud rate is slow enough that
   * its value is over 255 we'll have to switch to BRGH=0:
   *    SPBRG = Fint / (baud * 64) - 1
   */
  if (spbrg > 255) {
    spbrg = 24000000 / (baudRate * 64) - 1;
    brgh = 0x00;
  }

  rcpod_Poke(rcpod, RCPOD_REG_SPBRG, spbrg);        /* Set baud rate */
  rcpod_Poke(rcpod, RCPOD_REG_RCSTA, 0x90);         /* Enable the serial port */
  rcpod_Poke(rcpod, RCPOD_REG_TXSTA, 0x22 | brgh);  /* Enable async transmitter, set BRGH */
}


void rcpod_SerialTxRxStart(rcpod_dev* rcpod, unsigned char* buffer, int count) {
  if (count > RCPOD_SCRATCHPAD_SIZE) {
    rcpod_HandleError("rcpod_SerialTxRxStart", EINVAL, "Size of transmission exceeds scratchpad buffer size");
    return;
  }

  /* Load the transmission into the scratchpad, if we have one */
  if (count > 0)
    rcpod_PokeBuffer(rcpod, RCPOD_REG_SCRATCHPAD, buffer, count);

  /* Perform the transmission, and start the receive. Give it the entire
   * scratchpad as a receive buffer.
   */
  rcpod_UsartTxRx(rcpod, RCPOD_REG_SCRATCHPAD, count, RCPOD_SCRATCHPAD_SIZE);
}


void rcpod_SerialTx(rcpod_dev* rcpod, unsigned char* buffer, int count) {
  if (count > RCPOD_SCRATCHPAD_SIZE) {
    rcpod_HandleError("rcpod_SerialTx", EINVAL, "Size of transmission exceeds scratchpad buffer size");
    return;
  }

  /* Load the transmission into the scratchpad, if we have one */
  if (count > 0)
    rcpod_PokeBuffer(rcpod, RCPOD_REG_SCRATCHPAD, buffer, count);

  /* No receive */
  rcpod_UsartTxRx(rcpod, RCPOD_REG_SCRATCHPAD, count, 0);
}


void rcpod_SerialRxStart(rcpod_dev* rcpod) {
  /* No transmit, receive into the entire scratchpad */
  rcpod_UsartTxRx(rcpod, RCPOD_REG_SCRATCHPAD, 0, RCPOD_SCRATCHPAD_SIZE);
}


void rcpod_SerialRxFinish(rcpod_dev* rcpod) {
  /* No new transmit or receive, just cancel the current receive */
  rcpod_UsartTxRx(rcpod, RCPOD_REG_SCRATCHPAD, 0, 0);
}


int rcpod_SerialRxProgress(rcpod_dev* rcpod) {
  /* This function is actually a good bit more important than the API reference
   * makes it seem- it reads the firmware's rx_count counter using rcpod_UsartRxProgress,
   * however this counter is only 8-bit. We need to detect rollovers in that counter,
   * and update the number of unreceived bytes available.
   * Internally this function is called by rcpod_SerialRxRead.
   */
  int count8 = rcpod_UsartRxProgress(rcpod);
  int newBytes;

  newBytes = count8 - rcpod->rx.last_count8;
  if (newBytes < 0) {
    /* The rcpod's 8-bit count variable overflowed */
    newBytes += 256;
  }
  rcpod->rx.last_count8 = count8;

  /* Update total byte count and unread byte count */
  rcpod->rx.count += newBytes;
  rcpod->rx.unread += newBytes;

  /* Check for buffer overflows */
  if (rcpod->rx.unread > rcpod->rx.size) {
    rcpod_HandleError("rcpod_SerialRxProgress", EIO, "Internal serial receive buffer overflow");
    return -1;
  }

  return rcpod->rx.count;
}


int rcpod_SerialRxRead(rcpod_dev* rcpod, unsigned char* buffer, int count) {
  int retval = 0;

  /* First update the count information in rcpod->rx and check for overflows */
  rcpod_SerialRxProgress(rcpod);

  /* Now we can just funnel bytes from the rcpod's ring buffer into our provided buffer... */
  while (count > 0 && rcpod->rx.unread > 0) {
    int blockSize;

    /* Determine the maximum amount we can read in one contiguous block.
     * This is limited by the caller's buffer, the number of unread
     * bytes, and the number of bytes before the buffer wraps around.
     */
    blockSize = count;
    if (blockSize > rcpod->rx.unread)
      blockSize = rcpod->rx.unread;
    if (blockSize + rcpod->rx.tail_index > rcpod->rx.size)
      blockSize = rcpod->rx.size - rcpod->rx.tail_index;

    rcpod_PeekBuffer(rcpod, rcpod->rx.address + rcpod->rx.tail_index, buffer, blockSize);
    retval += blockSize;
    rcpod->rx.unread -= blockSize;
    rcpod->rx.tail_index += blockSize;
    rcpod->rx.tail_index &= rcpod->rx.size;
  }

  return retval;
}


int rcpod_SerialSetTxEnable(rcpod_dev* rcpod, rcpod_pin pin) {
  rcpod_UsartTxe(rcpod, pin);
}

/* The End */
