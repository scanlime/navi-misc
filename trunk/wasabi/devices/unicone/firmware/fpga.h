/*
 * fpga.h - Abstractions for communicating with our XC4010XL FPGA
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

#ifndef _H_FPGA
#define _H_FPGA

/* An arbitrary 32-character version code stored along with the FPGA
 * bitstream. This is sent preceeding the actual content of the bitstream.
 */
extern unsigned char fpga_version_stamp[32];

/* Reset the FPGA */
void             fpga_init();

/* Get the FPGA ready for programming */
void             fpga_config_begin();

/* Send configuration data to the FPGA */
void             fpga_config_write(unsigned char *cfgdata, int length);

/* Finish configuring the FPGA, returning a status code (UNICONE_STATUS_*) */
unsigned char    fpga_config_end();

/* Only return the FPGA status, without sending anything */
unsigned char    fpga_config_status();

#endif /* _H_FPGA */

/* The End */
