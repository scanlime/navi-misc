/*
 * i2c_driver.h - A relatively high level C interface to the TUSB3210/3410
 *                I2C master interface.
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

#ifndef _H_I2C_DRIVER
#define _H_I2C_DRIVER

void           i2c_write_byte (unsigned char b, bit is_last);
unsigned char  i2c_read_byte  (bit is_last);
void           i2c_start      (unsigned char addr);
int            i2c_status     (void);

#endif /* _H_I2C_DRIVER */

/* The End */
