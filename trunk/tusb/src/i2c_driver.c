/*
 * i2c_driver.c - A relatively high level C interface to the TUSB3210/3410
 *                I2C master interface.
 *
 * Copyright (C) 2004  Micah Dowty
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
#include <tusb.h>
#include "i2c_driver.h"

int i2c_error_flag = 0;

void i2c_write_byte(unsigned char b, bit is_last)
{
  I2CADR &= ~1;       /* Write */

  if (is_last)
    I2CSTA |= SWR;    /* Stop condition */
  else
    I2CSTA &= ~SWR;   /* No stop condition */

  I2CDAO = b;
  while ((I2CSTA & TXE) == 0) {
    if (I2CSTA & ERR) {
      I2CSTA |= ERR;
      i2c_error_flag = 1;
      return;
    }
  }
}

unsigned char i2c_read_byte(bit is_last)
{
  I2CADR |= 1;        /* Read */

  if (is_last)
    I2CSTA |= SRD;    /* Stop condition */
  else
    I2CSTA &= ~SRD;   /* No stop condition */
  I2CDAO = 0;         /* Dummy byte starts the read transfer */

  while ((I2CSTA & RXF) == 0) {
    if (I2CSTA & ERR) {
      I2CSTA |= ERR;
      i2c_error_flag = 1;
      return 0;
    }
  }
  return I2CDAI;
}

void i2c_start(unsigned char addr)
{
  i2c_error_flag = 0;

  I2CADR = (addr<<1);
  I2CSTA = ERR | S1_4;      /* Clear error flag, 400khz mode, no stop condition */

  if (I2CSTA & ERR)
    i2c_error_flag = 1;
}

/* Call after an operation is complete to retrieve the status flag and reset it */
int i2c_status(void)
{
  if (i2c_error_flag)
    return -1;

  return 0;
}

/* The End */
