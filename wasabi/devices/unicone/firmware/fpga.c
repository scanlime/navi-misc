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

#include <stdio.h>
#include <uart_driver.h>
#include <tusb.h>
#include <unicone.h>

static bit config_in_progress;
static long bytes_configured;

static void      fpga_config_write_byte(unsigned char c);
static void      fpga_init();
static void      fpga_program_on();
static void      fpga_program_off();


/**************************************************************************/
/********************************************** Low-level configuration ***/
/**************************************************************************/

#define CCLK P3_0
#define DIN  P3_2

/* The PROGRAM pin is connected to RTS on our serial port */
static void fpga_program_on()
{
  MCR |= MCR_RTS;
}

static void fpga_program_off()
{
  MCR &= ~MCR_RTS;
}

static void fpga_init()
{
  DIN = 0;
  CCLK = 0;
  fpga_program_on();
  fpga_program_off();
}

static void fpga_config_write_byte(unsigned char c)
{
  int i;

  /* Clock out bits left-to-right */
  for (i=8; i; i--) {
    DIN = (c & 0x80) ? 1 : 0;
    c <<= 1;
    CCLK = 1;
    CCLK = 0;
  }
}


/**************************************************************************/
/******************************************************* Public Methods ***/
/**************************************************************************/


void fpga_config_begin()
{
  config_in_progress = 1;
  bytes_configured = 0;

  fpga_init();

  printf("\nConfiguring FPGA:\n");
}

void fpga_config_write(unsigned char *cfgdata, int length)
{
  int i;
  const long dot_mask = ~0x3FF;

  if (!config_in_progress) {
    printf("fgpa_config_write without config_in_progress\n");
    return;
  }

  for (i=length; i; i--)
    fpga_config_write_byte(*(cfgdata++));

  /* Give a dot every time we cross a 1024-byte boundary */
  if ( ((bytes_configured + length) & dot_mask) !=
       (bytes_configured & dot_mask) )
    putchar('.');

  bytes_configured += length;
}

unsigned char fpga_config_end()
{
  config_in_progress = 0;

  /* Clock out a few more bits just to make sure the FPGA is in a sane state */
  fpga_config_write_byte(0x00);

  printf("\nDone, wrote %ld bytes\n\n", bytes_configured);
  return UNICONE_STATUS_OK;
}

/* The End */
