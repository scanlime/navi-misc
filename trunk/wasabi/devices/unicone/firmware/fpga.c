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
#include <util.h>

#include <unicone.h>
#include "hardware.h"

static bit config_in_progress;
static long bytes_configured;

static void      fpga_config_write_byte(unsigned char c);
static void      fpga_init();


/**************************************************************************/
/********************************************** Low-level configuration ***/
/**************************************************************************/

static void fpga_init()
{
  fpga_din_off();
  fpga_cclk_off();
  fpga_program_off();
  delay(1000);
  fpga_program_on();
}

static void fpga_config_write_byte(unsigned char c)
{
  int i;

  /* Clock out bits left-to-right */
  for (i=8; i; i--) {
    if (c & 0x80)
      fpga_din_on();
    else
      fpga_din_off();
    c <<= 1;
    watchdog_reset();
    fpga_cclk_on();
    watchdog_reset();
    fpga_cclk_off();
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

  printf("Configuring FPGA ");
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

  if (fpga_done()) {
    /* Give it a few more CCLKs after the bitstream to release user I/Os */
    fpga_config_write_byte(0x00);

    printf(" Done\n");
    return UNICONE_STATUS_OK;
  }
  else{
    printf(" Error\n");
    return UNICONE_STATUS_ERROR;
  }
}

/* The End */
