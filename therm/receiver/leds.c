/*
 * leds.c - Interface to the front panel LEDs, connected to the I2C
 *          bus using a MAX7300. We have a 'spinner' of four LEDs that
 *          indicates reception of complete packets, and an individual
 *          led that glows softly and indicates other kinds of activity.
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

#include <i2c_driver.h>
#include "therm-rx.h"

#define SPINNER_MAX_QUEUED 20
static int spinner_queue;
static int blink_duration;


static void max7300_write(unsigned char address, unsigned char data_byte)
{
  /* This is the I2C address of the LED controller */
  i2c_start(0x44);

  i2c_write_byte(address, 0);
  i2c_write_byte(data_byte, 1);
}

void leds_init()
{
  /* Take the MAX7300 out of shutdown */
  max7300_write(0x04, 0x01);

  /* All LEDs off */
  leds_set(0);

  /* All pins are outputs */
  max7300_write(0x09, 0x55);
  max7300_write(0x0A, 0x55);
  max7300_write(0x0B, 0x55);
  max7300_write(0x0C, 0x55);
  max7300_write(0x0D, 0x55);
  max7300_write(0x0E, 0x55);
  max7300_write(0x0F, 0x55);

  spinner_queue = 0;
  blink_duration = 0;
}

void leds_set(unsigned char bits)
{
  /* The LEDs are active-low */
  max7300_write(0x53, 0xFF ^ bits);
}

void leds_update()
{
  unsigned char leds;
  static int wait_cycle = 0;
  static unsigned char spinner_cycle = 0;

  if (spinner_queue) {

    wait_cycle++;
    if (wait_cycle == 200) {
      wait_cycle = 0;

      spinner_cycle++;
      if (spinner_cycle == 4) {
	spinner_cycle = 0;
	spinner_queue--;
      }
    }

    switch (spinner_cycle) {
    case 0:  leds = LED_SPINNER_TOP;     break;
    case 1:  leds = LED_SPINNER_RIGHT;   break;
    case 2:  leds = LED_SPINNER_BOTTOM;  break;
    case 3:  leds = LED_SPINNER_LEFT;    break;
    }
  }
  else {
    wait_cycle = 0;
    leds = 0;
    spinner_cycle = 0;
  }

  if (blink_duration)
    blink_duration--;
  else
    leds |= LED_MAIN;

  leds_set(leds);
}

void leds_blink()
{
  blink_duration = 200;
}

void leds_queue_spinner(int cycles)
{
  /* Add more cycles to the number pending in our spinner */
  spinner_queue += cycles;
  if (spinner_queue > SPINNER_MAX_QUEUED)
    spinner_queue = SPINNER_MAX_QUEUED;
}

/* The End */
