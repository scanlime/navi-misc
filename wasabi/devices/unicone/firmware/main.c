/*
 * main.c - Entry point and main loop for the Unicone firmware
 *
 * Universal Controller Emulator project
 * Copyright (C) 2004 Micah Dowty
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <stdio.h>
#include <string.h>

#include <usb_driver.h>
#include <uart_driver.h>
#include <util.h>

#include <unicone.h>
#include "hardware.h"
#include "fpga.h"
#include "i2c.h"


/* We reserve space starting at 0xFD00 for endpoint buffers.
 */
volatile xdata at 0xFD00 unsigned char ep1_out_x[64];
volatile xdata at 0xFD40 unsigned char ep2_out_x[64];


void i2c_write_packets(unsigned char *packets, int length)
{
  unsigned char wr_addr, wr_len;

  /* Write multiple I2C data packets */
  while (length > 2) {

    /* Read the header- one byte address, one byte length */
    wr_addr = *(packets++);
    wr_len = *(packets++);
    length -= 2;

    /* Validate the header */
    if (wr_len > length || wr_len < 1)
      return;

    /* Perform the write */
    length -= wr_len;
    i2c_start(wr_addr);
    while (wr_len > 1) {
      i2c_write_byte(*(packets++), 0);
      wr_len--;
    }
    i2c_write_byte(*(packets++), 1);
    if (i2c_status() < 0)
      printf("I2C error when setting LED brightness\n");
  }
}

void main()
{
  int c;

  uart_init();
  printf("\n---- Unicone firmware v%x.%02x ----\n",
	 UNICONE_VERSION >> 8, UNICONE_VERSION & 0xFF);

  /* This connects to the USB bus and initializes EP0 */
  usb_init();
  puts("USB initialized");

  /* Initialize bulk endpoints */
  usb_dma_unstall(EDB_OEP1);
  usb_dma_setup(EDB_OEP1, ep1_out_x, sizeof(ep1_out_x));
  usb_dma_unstall(EDB_OEP2);
  usb_dma_setup(EDB_OEP2, ep2_out_x, sizeof(ep2_out_x));

  while (1) {
    watchdog_reset();
    usb_poll();

    /* If we have data on EP1 OUT, configure the FPGA with it and look for more */
    c = usb_dma_status(EDB_OEP1);
    if (c) {
      fpga_config_write(ep1_out_x, c);
      usb_dma_setup(EDB_OEP1, ep1_out_x, sizeof(ep1_out_x));
    }

    /* If we have data on EP2_OUT, break it into I2C writes to execute, and look for more */
    c = usb_dma_status(EDB_OEP2);
    if (c) {
      i2c_write_packets(ep2_out_x, c);
      usb_dma_setup(EDB_OEP2, ep2_out_x, sizeof(ep2_out_x));
    }
  }
}

void usb_handle_vendor_request()
{
  static unsigned char retval = 0;

  switch (usb_setup_buffer.bRequest) {

  case UNICONE_REQ_FPGA_CONFIG_BEGIN:
    fpga_config_begin();
    usb_write_ack();
    break;

  case UNICONE_REQ_FPGA_CONFIG_END:
    retval = fpga_config_end();
    usb_write_ep0_buffer(&retval, 1);
    break;

  case UNICONE_REQ_FPGA_STATUS:
    retval = fpga_config_status();
    usb_write_ep0_buffer(&retval, 1);
    break;

  case UNICONE_REQ_REBOOT:
    usb_write_ack();
    fpga_config_begin();   /* Reset the FPGA too */
    while (1);             /* Wait for the WDT to reset us */
    break;

  default:
    printf("Unknown vendor request %d\n", usb_setup_buffer.bRequest);
  }
}

/* The End */
