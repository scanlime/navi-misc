/*
 * therm-rx-protocol.h - Definitions for the USB protocol used by
 *                       our receiver.
 *
 * Wireless therm system
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

#ifndef _H_THERM_RX_PROTOCOL
#define _H_THERM_RX_PROTOCOL


/******************************************************************************/
/*************************************************************** Device Info **/
/******************************************************************************/

#define THERMRX_VENDOR_ID        0xE461
#define THERMRX_PRODUCT_ID       0x0011
#define THERMRX_REVISION         0x0100

#define THERMRX_BOOT_PRODUCT_ID  0x0012
#define THERMRX_BOOT_ENDPOINT    0x01

/* This endpoint is where we receive validated radio packets.
 * Each packet is of the form:
 *   - Two bytes (little-endian) indicating the number of content bits received
 *   - Two bytes (little-endian) counting the number of sub-bit errors
 *   - Two bytes (little-endian) indicating the number of total bits received
 *   - The packet content, stored LSB-first and padded to the nearest byte
 */
#define THERMRX_PACKET_ENDPOINT  0x81

#define THERMRX_BUFFER_SIZE      64


/******************************************************************************/
/************************************************************** USB Requests **/
/******************************************************************************/

/* Lets the watchdog timer reboot the device, putting it back in bootloader mode */
#define THERMRX_REQ_REBOOT        0x01

/* Read the internal temperature sensor, returning a 1-byte packet */
#define THERMRX_REQ_LOCAL_TEMP    0x02


#endif /* _H_THERM_RX_PROTOCOL */

/* The End */
