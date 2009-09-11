/*
 * fastftdi.c - A minimal FTDI FT232H interface for which supports bit-bang
 *              mode, but focuses on very high-performance support for
 *              synchronous FIFO mode. Requires libusb-1.0
 *
 * Copyright (C) 2009 Micah Dowty
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string.h>
#include "fastftdi.h"


int
FTDIDevice_Open(FTDIDevice *dev)
{
  int err;
  int interface;

  memset(dev, 0, sizeof *dev);

  if ((err = libusb_init(&dev->libusb))) {
    return err;
  }

  libusb_set_debug(dev->libusb, 3);

  dev->handle = libusb_open_device_with_vid_pid(dev->libusb,
						FTDI_VENDOR,
						FTDI_PRODUCT_FT2232H);
  if (!dev->handle) {
    return LIBUSB_ERROR_NO_DEVICE;
  }

  for (interface = 0; interface < 2; interface++) {
    libusb_detach_kernel_driver(dev->handle, interface);
  }

  if ((err = libusb_set_configuration(dev->handle, 1))) {
    perror("Error setting configuration");
    return err;
  }

  for (interface = 0; interface < 2; interface++) {
    if ((err = libusb_claim_interface(dev->handle, interface))) {
      perror("Error claiming interface");
      return err;
    }
  }
  
  return 0;
}


void
FTDIDevice_Close(FTDIDevice *dev)
{
  libusb_close(dev->handle);
  libusb_exit(dev->libusb);
}


int
FTDIDevice_SetMode(FTDIDevice *dev, FTDIInterface interface,
		   FTDIBitmode mode, uint8_t pinDirections,
		   int baudRate)
{
  int err;

  err = libusb_control_transfer(dev->handle,
				LIBUSB_REQUEST_TYPE_VENDOR
				| LIBUSB_RECIPIENT_DEVICE
				| LIBUSB_ENDPOINT_OUT,
				FTDI_SET_BITMODE_REQUEST,
				pinDirections | (mode << 8),
				interface,
				NULL, 0,
				FTDI_COMMAND_TIMEOUT);
  if (err)
    return err;

  if (baudRate) {
    int divisor;

    if (mode == FTDI_BITMODE_BITBANG)
      baudRate <<= 2;

    divisor = 240000000 / baudRate;
    if (divisor < 1 || divisor > 0xFFFF) {
      return LIBUSB_ERROR_INVALID_PARAM;
    }

    err = libusb_control_transfer(dev->handle,
				  LIBUSB_REQUEST_TYPE_VENDOR
				  | LIBUSB_RECIPIENT_DEVICE
				  | LIBUSB_ENDPOINT_OUT,
				  FTDI_SET_BAUD_REQUEST,
				  divisor,
				  interface,
				  NULL, 0,
				  FTDI_COMMAND_TIMEOUT);
    if (err)
      return err;
  }
}


int
FTDIDevice_WriteSync(FTDIDevice *dev, FTDIInterface interface,
		     uint8_t *data, size_t length)
{
  int err, transferred;

  err = libusb_bulk_transfer(dev->handle, FTDI_EP_OUT(interface),
			     data, length, &transferred,
			     FTDI_COMMAND_TIMEOUT);
  if (err < 0)
    return err;
  else
    return 0;
}


int
FTDIDevice_WriteByteSync(FTDIDevice *dev, FTDIInterface interface, uint8_t byte)
{
  return FTDIDevice_WriteSync(dev, interface, &byte, sizeof byte);
}


int
FTDIDevice_ReadByteSync(FTDIDevice *dev, FTDIInterface interface, uint8_t *byte)
{
  /*
   * This is a simplified synchronous read, intended for bit-banging mode.
   * Ignores the modem/buffer status bytes, returns just the data.
   */
  
  uint8_t packet[FTDI_PACKET_SIZE];
  int transferred, err;

  err = libusb_bulk_transfer(dev->handle, FTDI_EP_IN(interface),
			     packet, sizeof packet, &transferred,
			     FTDI_COMMAND_TIMEOUT);
  if (err < 0) {
    return err;
  }
  if (transferred != sizeof packet) {
    return -1;
  }

  *byte = packet[FTDI_HEADER_SIZE];

  return 0;
}
