/*
 * unicone.h - Shared definitions for the Unicone firmware and userspace code
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

#ifndef _H_UNICONE
#define _H_UNICONE

/**************************************************************************/
/*********************************************************** Identity *****/
/**************************************************************************/

#define UNICONE_VENDOR_ID   0xE461
#define UNICONE_PRODUCT_ID  0x000C
#define UNICONE_VERSION     0x0100

#define UNICONE_BOOTLOAD_VENDOR_ID   0xE461
#define UNICONE_BOOTLOAD_PRODUCT_ID  0x000B


/**************************************************************************/
/********************************************************** Endpoints *****/
/**************************************************************************/

/* Endpoint for sending firmware, followed by a two-byte length
 * and one-byte checksum. (simple 8-bit sum of all bytes in firmware)
 * This is handled by the bootloader (stored in ROM) and only valid when
 * the device shows up with the special booloader vendor and product IDs.
 */
#define UNICONE_EP_BOOTLOAD     1

/* Endpoint for sending FPGA configuration, between UNICONE_REQ_FPGA_CONFIG_BEGIN
 * and UNICONE_REQ_FPGA_CONFIG_END requests.
 */
#define UNICONE_EP_FPGA_CONFIG  1

/* Endpoint for sending arbitrary data to the FPGA over I2C.
 * Send one or more packets consisting of:
 *  1. The I2C address (defined by the FPGA) to write to (one byte)
 *  2. The length of the write (one byte)
 *  3. An appropriate number of data bytes
 */
#define UNICONE_EP_FPGA_WRITE   2


/**************************************************************************/
/******************************************************** I2C Devices *****/
/**************************************************************************/

/* Write 16-bit big-endian values to change the LED brightness */
#define UNICONE_I2C_LED         0x21


/**************************************************************************/
/**************************************************** Vendor Requests *****/
/**************************************************************************/

/* Start programming the FPGA. This has the side effect of putting it into
 * an unconfigured state and zero'ing its current contents.
 *
 * To complete configuration, the host needs to send, on UNICONE_EP_FPGA_CONFIG:
 *   1. Send a 20-byte SHA-1 version stamp for the bitstream. This isn't checked
 *      by the firmware (the bitstream itself has builtin checksums) but it can
 *      be retrieved later to determine what version of the bitstream is installed.
 *   2. Send the complete bitstream. It will be clocked out as-is to the FPGA
 *
 * Then the host sends a UNICONE_REQ_FPGA_CONFIG_END and reads the resulting status.
 */
#define UNICONE_REQ_FPGA_CONFIG_BEGIN    0x11

/* Finish programming the FPGA. This finishes clocking the bitstream
 * into the FPGA and checks the 'DONE' pin. Returns a 1-byte status code.
 */
#define UNICONE_REQ_FPGA_CONFIG_END      0x12

/* Check the FPGA programming status, returns UNICONE_STATUS_OK if the FPGA
 * is programmed, or UNICONE_STATUS_ERROR if it has not yet been successfully
 * configured.
 */
#define UNICONE_REQ_FPGA_STATUS          0x13

/* These two command return the 20-byte SHA-1 digests of the current firmware
 * and the current FPGA configuration, respectively.
 */
#define UNICONE_REQ_GET_FIRMWARE_STAMP   0x20
#define UNICONE_REQ_GET_FPGA_STAMP       0x21

/* Force us to reboot back into the TUSB bootloader */
#define UNICONE_REQ_REBOOT               0x34


/**************************************************************************/
/******************************************************* Status Codes *****/
/**************************************************************************/

#define UNICONE_STATUS_OK                0x00
#define UNICONE_STATUS_ERROR             0x01
#define UNICONE_STATUS_IN_PROGRESS       0x02


#endif /* _H_UNICONE */

/* The End */
