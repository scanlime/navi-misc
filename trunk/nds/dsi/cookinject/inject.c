/*
 * Payload for the 'cookinject' exploit.
 *
 * This code is loaded from SPI memory by the bootstrapper in
 * 'exploit.s'.  Our main() is called. When it returns, we return
 * control to the game.
 *
 * Copyright (C) 2009 Micah Dowty <micah@navi.cx>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <string.h>
#include <nds.h>
#include "spime.h"
#include "logo.h"

static void setupLogo();

int foo[1024];

void main()
{
   setupLogo();

   /* RAM dump */
   //spimeWrite(0x10000, MAINRAM8 + 0x150000, 0xF0000);
}


void setupLogo()
{
   /*
    * The game already drew a background image on the top screen.
    * Replace it with our logo. It hasn't drawn the bottom screen
    * yet, so we can't do much about that right now.
    */
   decompress(logoPal, BG_PALETTE, RLEVram);
   decompress(logoBitmap, BG_GFX, RLEVram);

   /*
    * After we return, the game is going to try and display a
    * "Welcome" message on the screen. That message is mostly
    * garbage at this point, since it contained our player name
    * from the save file- and we'd rather it not scribble all
    * over our pretty logo. So stub out the beginning of the
    * buffer that holds the welcome message.
    */
   MAINRAM8[0x20c310] = '\0';

   /*
    * Make the logo visible immediately. We don't have to do this,
    * since the game will fade both screens in.  but it helps with
    * debugging.
    */
   REG_MASTER_BRIGHT = 0;
}
