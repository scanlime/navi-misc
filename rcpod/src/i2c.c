/*
 * i2c.c - I2C bus master implementation. Firmware-accelerated if it's supported,
 *         otherwise implemented here with GPIO twiddling.
 *
 * Remote Controlled PIC of Doom
 * Copyright (C) 2003-2004 Micah Dowty <micah@navi.cx>
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

static void          softi2c_start      (rcpod_dev *rcpod, rcpod_i2c_dev *idev);
static void          softi2c_stop       (rcpod_dev *rcpod, rcpod_i2c_dev *idev);
static void          softi2c_set_data   (rcpod_dev *rcpod, rcpod_i2c_dev *idev, int value);
static int           softi2c_test_data  (rcpod_dev *rcpod, rcpod_i2c_dev *idev);
static void          softi2c_set_clock  (rcpod_dev *rcpod, rcpod_i2c_dev *idev, int value);

static unsigned char softi2c_read_byte  (rcpod_dev *rcpod, rcpod_i2c_dev *idev,
					 int ack);
static int           softi2c_write_byte (rcpod_dev *rcpod, rcpod_i2c_dev *idev,
					 unsigned char byte);

static int           softi2c_write      (rcpod_dev *rcpod, rcpod_i2c_dev *idev,
					 const unsigned char* write_buffer, int write_count);
static int           softi2c_read       (rcpod_dev *rcpod, rcpod_i2c_dev *idev,
					 unsigned char* write_buffer, int read_count);
static int           softi2c_write_read (rcpod_dev *rcpod, rcpod_i2c_dev *idev,
					 const unsigned char* write_buffer, int write_count,
					 unsigned char* read_buffer, int read_count);


/*************************************************************************************/
/************************************************* Host-side software implementation */
/*************************************************************************************/

/* This is a slow I2C layer built on top of the GPIO functions.
 * It's only used when the rcpod's firmware is old enough that
 * it can't do I2C on its own. The write and read functions
 * all return the number of successful ACKs, just like the
 * public interface does.
 */

static void softi2c_set_data(rcpod_dev *rcpod, rcpod_i2c_dev *idev, int value) {
  /* Our data line is open collector- when it's high, it's high impedance.
   * When it's low, it actually drives the output low.
   */
  if (value) {
    rcpod_GpioAssert(rcpod, RCPOD_INPUT(idev->data));
  }
  else {
    rcpod_pin pins[] = {
      RCPOD_NEGATE(idev->data),
      RCPOD_OUTPUT(idev->data),
    };
    rcpod_GpioAssertBuffer(rcpod, pins, sizeof(pins)/sizeof(pins[0]));
  }
}

static int softi2c_test_data(rcpod_dev *rcpod, rcpod_i2c_dev *idev) {
  return rcpod_GpioTest(rcpod, idev->data);
}

static void softi2c_set_clock(rcpod_dev *rcpod, rcpod_i2c_dev *idev, int value) {
  if (value)
    rcpod_GpioAssert(rcpod, idev->clock);
  else
    rcpod_GpioAssert(rcpod, RCPOD_NEGATE(idev->clock));
}

static void softi2c_start(rcpod_dev *rcpod, rcpod_i2c_dev *idev) {
  /* Ensure our clock line is an output */
  rcpod_GpioAssert(rcpod, RCPOD_OUTPUT(idev->clock));

  softi2c_set_clock(rcpod, idev, 1);
  softi2c_set_data(rcpod, idev, 1);
  softi2c_set_data(rcpod, idev, 0);
  softi2c_set_clock(rcpod, idev, 0);
}

static void softi2c_stop(rcpod_dev *rcpod, rcpod_i2c_dev *idev) {
  softi2c_set_data(rcpod, idev, 0);
  softi2c_set_clock(rcpod, idev, 1);
  softi2c_set_data(rcpod, idev, 1);
}

static unsigned char softi2c_read_byte(rcpod_dev *rcpod, rcpod_i2c_dev *idev,
				       int ack) {
  int bit, byte;

  /* Clock in each bit */
  byte = 0;
  softi2c_set_data(rcpod, idev, 1);
  for (bit=0; bit<8; bit++) {
    byte <<= 1;
    softi2c_set_clock(rcpod, idev, 1);
    if (softi2c_test_data(rcpod, idev))
      byte |= 1;
    softi2c_set_clock(rcpod, idev, 0);
  }

  /* Clock out our ACK */
  softi2c_set_data(rcpod, idev, !ack);
  softi2c_set_clock(rcpod, idev, 1);
  softi2c_set_clock(rcpod, idev, 0);
}

static int softi2c_write_byte(rcpod_dev *rcpod, rcpod_i2c_dev *idev,
			      unsigned char byte) {
  int bit, ack;

  /* Clock out each bit */
  for (bit=0; bit<8; bit++) {
    softi2c_set_data(rcpod, idev, byte & 0x80);
    byte <<= 1;
    softi2c_set_clock(rcpod, idev, 1);
    softi2c_set_clock(rcpod, idev, 0);
  }

  /* Clock in our ACK */
  softi2c_set_clock(rcpod, idev, 1);
  softi2c_set_data(rcpod, idev, 1);
  ack = !softi2c_test_data(rcpod, idev);
  softi2c_set_clock(rcpod, idev, 0);
  return ack;
}

static int softi2c_write(rcpod_dev *rcpod, rcpod_i2c_dev *idev,
			 const unsigned char* write_buffer, int write_count) {
  int i;

  /* Start a transaction and write our address, without the R bit set */
  softi2c_start(rcpod, idev);
  if (!softi2c_write_byte(rcpod, idev, idev->address << 1)) {
    softi2c_stop(rcpod, idev);
    return 0;
  }

  /* Write each byte, giving up if we don't get an ACK */
  for (i=0; i<write_count; i++)
    if (!softi2c_write_byte(rcpod, idev, write_buffer[i])) {
      /* No ACK */
      softi2c_stop(rcpod, idev);
      return i+1;
    }

  softi2c_stop(rcpod, idev);
  return write_count+1;
}

static int softi2c_read(rcpod_dev *rcpod, rcpod_i2c_dev *idev,
			unsigned char* read_buffer, int read_count) {
  int i;

  /* Start a transaction and write our address, with the R bit set */
  softi2c_start(rcpod, idev);
  if (!softi2c_write_byte(rcpod, idev, (idev->address << 1) | 1)) {
    softi2c_stop(rcpod, idev);
    return 0;
  }

  /* Read each byte, ACK'ing all but the last */
  for (i=0; i<read_count; i++)
    read_buffer[i] = softi2c_read_byte(rcpod, idev, i < (read_count-1));

  /* End the transaction, return one ACK received. */
  softi2c_stop(rcpod, idev);
  return 1;
}

static int softi2c_write_read(rcpod_dev *rcpod, rcpod_i2c_dev *idev,
			      const unsigned char* write_buffer, int write_count,
			      unsigned char* read_buffer, int read_count) {
  int ack_total;
  ack_total = softi2c_write(rcpod, idev, write_buffer, write_count);
  if (ack_total == write_count+1)
    ack_total += softi2c_read(rcpod, idev, read_buffer, read_count);
  return ack_total;
}


/*************************************************************************************/
/************************************************** Public I2C entry points **********/
/*************************************************************************************/

int rcpod_I2CWriteRead(rcpod_dev* rcpod, rcpod_i2c_dev* idev,
		       const unsigned char* write_buffer, int write_count,
		       unsigned char* read_buffer, int read_count) {
  return softi2c_write_read(rcpod, idev, write_buffer, write_count, read_buffer, read_count);
}

int rcpod_I2CWrite(rcpod_dev* rcpod, rcpod_i2c_dev* idev,
		   const unsigned char* write_buffer, int write_count) {
  return softi2c_write(rcpod, idev, write_buffer, write_count);
}

int rcpod_I2CRead(rcpod_dev* rcpod, rcpod_i2c_dev* idev,
		  unsigned char* read_buffer, int read_count) {
  return softi2c_read(rcpod, idev, read_buffer, read_count);
}

/* The End */
