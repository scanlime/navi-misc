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
#include <i2c_driver.h>
#include <util.h>

#include <unicone.h>
#include "hardware.h"
#include "fpga.h"

/* We reserve space starting at 0xFD00 for endpoint buffers.
 */
volatile xdata at 0xFD00 unsigned char ep1_out_x[64];
volatile xdata at 0xFD40 unsigned char ep2_out_x[64];

/* Our firmware gets a version stamp, so we can identify it later
 * and only upgrade if necessary. This points to the very end of
 * code space- an SHA-1 digest is placed there by the upload utility.
 */
volatile code at 0x1FEC unsigned char firmware_version_stamp[20] = "";

unsigned short led_brightness;
unsigned short led_decay_rate;

void frequent_updates();
void infrequent_updates();
void i2c_write_packets(unsigned char *packets, int length);
void led_update();
void show_firmware_version();


/******************************************************************************/
/***************************************************************** Main Loop **/
/******************************************************************************/

void main()
{
  uart_init();
  show_firmware_version();

  usb_init();
  puts("USB initialized");
  fpga_init();
  puts("FPGA initialized");

  /* Initialize bulk endpoints */
  usb_dma_unstall(EDB_OEP1);
  usb_dma_setup(EDB_OEP1, ep1_out_x, sizeof(ep1_out_x));
  usb_dma_unstall(EDB_OEP2);
  usb_dma_setup(EDB_OEP2, ep2_out_x, sizeof(ep2_out_x));

  /* This chip doesn't have have a timer, so we just divide
   * updates into 'frequent' and 'infrequent'- frequent updates are run
   * several times for every infrequent update.
   */
  while (1) {
    unsigned char i;
    infrequent_updates();
    for (i=100;i;i--)
      frequent_updates();
  }
}

void infrequent_updates()
{
  led_update();
}

void frequent_updates()
{
  int c;

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

void show_firmware_version()
{
  int i;
  printf("\n---- Unicone firmware v%x.%02x [",
	 UNICONE_VERSION >> 8, UNICONE_VERSION & 0xFF);
  for (i=0; i<sizeof(firmware_version_stamp); i++)
    printf("%02x", firmware_version_stamp[i]);
  printf("] ----\n");
}


/******************************************************************************/
/************************************************************** USB Requests **/
/******************************************************************************/

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

  case UNICONE_REQ_GET_FIRMWARE_STAMP:
    usb_write_ep0_buffer(firmware_version_stamp, sizeof(firmware_version_stamp));
    break;

  case UNICONE_REQ_GET_FPGA_STAMP:
    usb_write_ep0_buffer(fpga_version_stamp, sizeof(fpga_version_stamp));
    break;

  case UNICONE_REQ_REBOOT:
    usb_write_ack();
    fpga_init();
    while (1);             /* Wait for the WDT to reset us */
    break;

  case UNICONE_REQ_SET_LED:
    led_brightness = usb_setup_buffer.wValue;
    led_decay_rate = usb_setup_buffer.wIndex;
    usb_write_ack();
    break;

  default:
    printf("Unknown vendor request %d\n", usb_setup_buffer.bRequest);
  }
}

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
      printf("I2C error\n");
  }
}

void led_update()
{
  static int prev_brightness = 0;

  /* Decay the LED brightness linearly each time through
   * the infrequent updates loop. This is crappy, but we
   * don't have any timer hardware on chip.
   */
  if (led_brightness > led_decay_rate)
    led_brightness -= led_decay_rate;
  else
    led_brightness = 0;

  /* Update the LED brightness over I2C, but only if the FPGA
   * might be listening and the brightness actually changed.
   */
  if (fpga_done() && led_brightness != prev_brightness) {
    i2c_start(UNICONE_I2C_LED);
    i2c_write_byte(led_brightness >> 8, 0);
    i2c_write_byte(led_brightness & 0xFF, 1);

    prev_brightness = led_brightness;
  }
}

/* The End */
