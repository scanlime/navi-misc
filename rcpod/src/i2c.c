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

static int           i2c_set_dev        (rcpod_dev *rcpod, rcpod_i2c_dev *idev);

static int           i2c_write_read     (rcpod_dev *rcpod,
					 const unsigned char* write_buffer, int write_count,
					 unsigned char* read_buffer, int read_count);
static int           i2c_write1_read8   (rcpod_dev *rcpod,
					 unsigned char write_byte,
					 unsigned char* read_buffer, int read_count);
static int           i2c_read8          (rcpod_dev *rcpod,
					 unsigned char* read_buffer, int read_count);
static int           i2c_write4         (rcpod_dev *rcpod,
					 const unsigned char* write_buffer, int write_count);


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

  return byte;
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
/************************************************** Low-level I2C commands ***********/
/*************************************************************************************/

static int i2c_set_dev(rcpod_dev *rcpod, rcpod_i2c_dev *idev) {
  /* Set the current I2C device, returns nonzero on error */
  int retval;

  /* Nothing to change? */
  if (rcpod->last_i2c_dev_valid &&
      memcmp(&rcpod->last_i2c_dev, idev, sizeof(rcpod_i2c_dev))==0)
    return;

  retval = usb_control_msg(rcpod->usbdevh, USB_TYPE_VENDOR | USB_ENDPOINT_OUT,
			   RCPOD_CTRL_I2C_SET,
			   (idev->clock << 8) | idev->data,
			   (idev->speed << 8) | idev->address,
			   NULL, 0, RCPOD_TIMEOUT);
  if (retval < 0) {
    rcpod_HandleError("i2c_set_dev", errno, strerror(errno));
  }
  else {
    memcpy(&rcpod->last_i2c_dev, idev, sizeof(rcpod_i2c_dev));
    rcpod->last_i2c_dev_valid = 1;
  }

  return retval;
}

static int i2c_write_read(rcpod_dev *rcpod,
			  const unsigned char* write_buffer, int write_count,
			  unsigned char* read_buffer, int read_count) {
  unsigned char ack_count;
  int retval;

  /* Check buffer sizes before we do anything */
  if (write_count > RCPOD_SCRATCHPAD_SIZE) {
    rcpod_HandleError("i2c_write_read", EINVAL, "Size of I2C write exceeds scratchpad buffer size");
    return 0;
  }
  if (read_count > RCPOD_SCRATCHPAD_SIZE) {
    rcpod_HandleError("i2c_write_read", EINVAL, "Size of I2C read exceeds scratchpad buffer size");
    return 0;
  }

  /* Write the transmit buffer to the scratchpad */
  if (write_count > 0)
    rcpod_PokeBuffer(rcpod, RCPOD_REG_SCRATCHPAD, write_buffer, write_count);

  retval = usb_control_msg(rcpod->usbdevh, USB_TYPE_VENDOR | USB_ENDPOINT_IN,
			   RCPOD_CTRL_I2C_TXRX,
			   (read_count << 8) | write_count,
			   RCPOD_REG_SCRATCHPAD,
			   &ack_count, 1, RCPOD_TIMEOUT);
  if (retval < 0) {
    rcpod_HandleError("i2c_write_read", errno, strerror(errno));
    return 0;
  }

  /* Read the scratchpad back into the receive buffer */
  if (read_count > 0)
    rcpod_PeekBuffer(rcpod, RCPOD_REG_SCRATCHPAD, read_buffer, read_count);

  return ack_count;
}

static int i2c_write1_read8(rcpod_dev *rcpod,
			    unsigned char write_byte,
			    unsigned char* read_buffer, int read_count) {
  int retval;

  retval = usb_control_msg(rcpod->usbdevh, USB_TYPE_VENDOR | USB_ENDPOINT_IN,
			   RCPOD_CTRL_I2C_W1READ8,
			   write_byte, read_count,
			   read_buffer, read_count, RCPOD_TIMEOUT);
  if (retval < 0) {
    rcpod_HandleError("i2c_write1_read8", errno, strerror(errno));
    return 0;
  }
  else if (retval == 0) {
    return 0;
  }

  /* One ACK for each address byte and one for the one write byte */
  return 3;
}

static int i2c_read8(rcpod_dev *rcpod,
		     unsigned char* read_buffer, int read_count) {
  int retval;

  retval = usb_control_msg(rcpod->usbdevh, USB_TYPE_VENDOR | USB_ENDPOINT_IN,
			   RCPOD_CTRL_I2C_READ8,
			   read_count, 0,
			   read_buffer, read_count, RCPOD_TIMEOUT);
  if (retval < 0) {
    rcpod_HandleError("i2c_read8", errno, strerror(errno));
    return 0;
  }
  else if (retval == 0) {
    return 0;
  }

  /* One ACK for the read's address byte */
  return 1;
}

static int i2c_write4(rcpod_dev *rcpod,
		      const unsigned char* write_buffer, int write_count) {
  int retval, cmd;
  unsigned char ack_count;

  if (write_count < 0) write_count = 0;
  if (write_count > 4) write_count = 4;

  switch (write_count) {
  case 0:  cmd = RCPOD_CTRL_I2C_WRITE0;  break;
  case 1:  cmd = RCPOD_CTRL_I2C_WRITE1;  break;
  case 2:  cmd = RCPOD_CTRL_I2C_WRITE2;  break;
  case 3:  cmd = RCPOD_CTRL_I2C_WRITE3;  break;
  case 4:  cmd = RCPOD_CTRL_I2C_WRITE4;  break;
  }

  retval = usb_control_msg(rcpod->usbdevh, USB_TYPE_VENDOR | USB_ENDPOINT_IN, cmd,
			   write_buffer[0] | (((int)write_buffer[1])<<8),
			   write_buffer[2] | (((int)write_buffer[3])<<8),
			   &ack_count, 1, RCPOD_TIMEOUT);
  if (retval < 0) {
    rcpod_HandleError("i2c_write4", errno, strerror(errno));
    return 0;
  }
  return ack_count;
}


/*************************************************************************************/
/************************************************** Public I2C entry points **********/
/*************************************************************************************/

int rcpod_HasAcceleratedI2C(rcpod_dev* rcpod) {
  /* We always have accelerated I2C if the firmware is version 1.20 or later. */
  return usb_device(rcpod->usbdevh)->descriptor.bcdDevice >= 0x0120;
}

int rcpod_I2CWriteRead(rcpod_dev* rcpod, rcpod_i2c_dev* idev,
		       const unsigned char* write_buffer, int write_count,
		       unsigned char* read_buffer, int read_count) {
  if (!rcpod_HasAcceleratedI2C(rcpod))
    return softi2c_write_read(rcpod, idev, write_buffer, write_count, read_buffer, read_count);

  if (i2c_set_dev(rcpod, idev))
    return 0;

  if (write_count == 1 && read_count <= 8)
    return i2c_write1_read8(rcpod, write_buffer[0], read_buffer, read_count);

  return i2c_write_read(rcpod, write_buffer, write_count, read_buffer, read_count);
}

int rcpod_I2CWrite(rcpod_dev* rcpod, rcpod_i2c_dev* idev,
		   const unsigned char* write_buffer, int write_count) {
  if (!rcpod_HasAcceleratedI2C(rcpod))
    return softi2c_write(rcpod, idev, write_buffer, write_count);

  if (i2c_set_dev(rcpod, idev))
    return 0;

  if (write_count <= 4)
    return i2c_write4(rcpod, write_buffer, write_count);

  return i2c_write_read(rcpod, write_buffer, write_count, NULL, 0);
}

int rcpod_I2CRead(rcpod_dev* rcpod, rcpod_i2c_dev* idev,
		  unsigned char* read_buffer, int read_count) {
  if (!rcpod_HasAcceleratedI2C(rcpod))
    return softi2c_read(rcpod, idev, read_buffer, read_count);

  if (i2c_set_dev(rcpod, idev))
    return 0;

  if (read_count <= 8)
    return i2c_read8(rcpod, read_buffer, read_count);

  return i2c_write_read(rcpod, NULL, 0, read_buffer, read_count);
}

/* The End */
