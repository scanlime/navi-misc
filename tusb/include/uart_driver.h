/*
 * uart_driver.h - A driver for the TUSB's UART, compatible with
 *                 SDCC's stdio implementation.
 *
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

#ifndef _H_UART_DRIVER
#define _H_UART_DRIVER

#define UART_BAUD_TO_DIVISOR(baud)  ((int)(14.76923077e6 / ((baud) * 16) + 0.5))

/* Initialization */
void uart_init       ();
void uart_set_speed  (int divisor);

/* blocking I/O */
void uart_write      (char c);
char uart_read       ();

/* Nonblocking I/O (Returns -1 if the operation can't be completed yet) */
int  uart_nb_write   (char c);
int  uart_nb_read    ();

/* stdio compatibility */
void putchar(char c);

#endif /* _H_UART_DRIVER */

/* The End */
