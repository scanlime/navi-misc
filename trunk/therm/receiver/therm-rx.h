/*
 * therm-rx.h - Local definitions for functions exported by our
 *              firmware modules.
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

#ifndef __THERM_RX_H
#define __THERM_RX_H

#define LED_MAIN           0x01
#define LED_SPINNER_TOP    0x02
#define LED_SPINNER_LEFT   0x04
#define LED_SPINNER_BOTTOM 0x08
#define LED_SPINNER_RIGHT  0x10

void leds_init();
void leds_set(unsigned char bits);
void leds_update();
void leds_queue_spinner(int cycles);
void leds_blink();

void receiver_init();
void receiver_set_buffer(unsigned char *buffer, int buffer_size);
int  receiver_poll();   /* Returns a length count once a complete packet is ready, otherwise zero */

void          tc74_init();
unsigned char tc74_read();

#endif /* __THERM_RX_H */

/* The End */
