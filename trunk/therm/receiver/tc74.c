/*
 * tc74.c - An interface to the receiver's local TC74 temperature sensor
 *
 * Wireless therm system
 * Copyright (C) 2004-2005 Micah Dowty
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

#include <i2c_driver.h>

#define TC74_ADDRESS (0x48 | 5)

void tc74_init()
{
  i2c_start(TC74_ADDRESS);
  i2c_write_byte(0x01, 0);  /* config register */
  i2c_write_byte(0x00, 1);  /* out of shutdown */
}

unsigned char tc74_read()
{
  i2c_start(TC74_ADDRESS);
  i2c_write_byte(0x00, 1);  /* temperature register */

  i2c_start(TC74_ADDRESS);
  return i2c_read_byte(1);
}

/* The End */
