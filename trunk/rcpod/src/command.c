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
  retval = usb_control_msg(rcpod->usbdevh, USB_TYPE_VENDOR, RCPOD_CTRL_POKE,
			   txBytes | (((int)rxBytes) << 8), address, NULL, 0, RCPOD_TIMEOUT);
  if (retval < 0)
    rcpod_HandleError("rcpod_UsartTxRx", errno, strerror(errno));
}


int rcpod_UsartRxEnd(rcpod_dev* rcpod) {
  int retval;
  unsigned char byteCount;
  retval = usb_control_msg(rcpod->usbdevh, USB_TYPE_VENDOR | USB_ENDPOINT_IN,
			   RCPOD_CTRL_USART_RX_END, 0, 0, (char*) &byteCount, 1, RCPOD_TIMEOUT);
  if (retval < 0) {
    rcpod_HandleError("rcpod_UsartRxEnd", errno, strerror(errno));
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
  retval = usb_control_msg(rcpod->usbdevh, USB_TYPE_VENDOR, RCPOD_CTRL_POKE,
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

/* The End */
