/*
 * This is an lcdproc driver for the Media Infrawidget 6000's VFD.
 * The VFD itself is an IEE Century 122-09220, but we talk to it through
 * a USB interface and kernel module that have a few extra features.
 *
 * This driver is based on the IEE Century driver by Ben Severson,
 * which was in turn based on the CFontz driver.
 *
 * configfile support added by Rene Wagner (c) 2001
 * backlight support modified by Rene Wagner (c) 2001
 * block patch by Eddie Sheldrake (c) 2001 inserted by Rene Wagner
 * big num patch by Luis Llorente (c) 2002
 * MI6K support by Micah Dowty (c) 2004
 *
 * Copyright (C) 2002-2004
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 =
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "lcd.h"
#include "mi6k.h"
#include "report.h"
#include "lcd_lib.h"

static int custom = 0;
typedef enum {
	hbar = 1,
	vbar = 2,
	bign = 4,
	beat = 8
} custom_type;

static FILE *dev;
static char *framebuf = NULL;
static int width = 0;
static int height = 0;
static int cellwidth = 5;
static int cellheight = 7;
static int brightness = DEFAULT_BRIGHTNESS;
static int offbrightness = DEFAULT_OFFBRIGHTNESS;

// Vars for the server core
MODULE_EXPORT char *api_version = API_VERSION;
MODULE_EXPORT int stay_in_foreground = 1;
MODULE_EXPORT int supports_multiple = 0;
MODULE_EXPORT char *symbol_prefix = "mi6k_";


// Internal functions
static void mi6k_hidecursor ();
static void mi6k_init_vbar (Driver * drvthis);

/////////////////////////////////////////////////////////////////
// Opens the kernel module's mi6k device node
//
MODULE_EXPORT int
mi6k_init (Driver * drvthis, char *args)
{
	int w, h;

	char device[200] = DEFAULT_DEVICE;
	char size[200] = DEFAULT_SIZE;

	debug(RPT_INFO, "mi6k: init(%p,%s)", drvthis, args );

	/*Read config file*/

	/*Which serial device should be used*/
	strncpy(device, drvthis->config_get_string ( drvthis->name , "Device" , 0, DEFAULT_DEVICE),sizeof(device));
	device[sizeof(device)-1]=0;
	debug (RPT_INFO,"mi6k: Using device: %s", device);

	/*Which size*/
	strncpy(size, drvthis->config_get_string ( drvthis->name , "Size" , 0 , DEFAULT_SIZE),sizeof(size));
	size[sizeof(size)-1]=0;
	if( sscanf(size , "%dx%d", &w, &h ) != 2
	|| (w <= 0) || (w > LCD_MAX_WIDTH)
	|| (h <= 0) || (h > LCD_MAX_HEIGHT)) {
		report (RPT_WARNING, "mi6k_init: Cannot read size: %s. Using default value.\n", size);
		sscanf( DEFAULT_SIZE , "%dx%d", &w, &h );
	} else {
		width = w;
		height = h;
	}

	/* Which backlight brightness */
	if (0<=drvthis->config_get_int ( drvthis->name , "Brightness" , 0 , DEFAULT_BRIGHTNESS) &&
	    drvthis->config_get_int ( drvthis->name , "Brightness" , 0 , DEFAULT_BRIGHTNESS) <= 7) {
		brightness = drvthis->config_get_int ( drvthis->name , "Brightness" , 0 , DEFAULT_BRIGHTNESS);
	} else {
		report (RPT_WARNING, "mi6k_init: Brightness must between 0 and 7. Using default value.\n");
	}

	/* Which backlight-off "brightness" */
	if (0<=drvthis->config_get_int ( drvthis->name , "OffBrightness" , 0 , DEFAULT_OFFBRIGHTNESS) &&
	    drvthis->config_get_int ( drvthis->name , "OffBrightness" , 0 , DEFAULT_OFFBRIGHTNESS) <= 255) {
		offbrightness = drvthis->config_get_int ( drvthis->name , "OffBrightness" , 0 , DEFAULT_OFFBRIGHTNESS);
	} else {
		report (RPT_WARNING, "mi6k_init: OffBrightness must between 0 and 255. Using default value.\n");
	}

	/* Open the mi6k device node. We use stdio's buffered I/O, to
	 * increase efficiency by flushing output only when necessary.
	 */
	debug( RPT_DEBUG, "mi6k: Opening device: %s", device);
	dev = fopen(device, "w");
	if (!dev) {
		report (RPT_ERR, "mi6k_init: can't open %s, %s\n", device, strerror(errno));
		return -1;
	}

	/* Make sure the frame buffer is there... */
	framebuf = (unsigned char *) malloc (width * height);
	memset (framebuf, ' ', width * height);

	mi6k_hidecursor();
	mi6k_backlight(drvthis, 1);
	mi6k_flush(drvthis);

	report (RPT_DEBUG, "mi6k_init: done\n");

	return 0;
}

/////////////////////////////////////////////////////////////////
// Clean-up
//
MODULE_EXPORT void
mi6k_close (Driver * drvthis)
{
	fclose(dev);

	if(framebuf) free (framebuf);
	framebuf = NULL;
}

/////////////////////////////////////////////////////////////////
// Returns the display width
//
MODULE_EXPORT int
mi6k_width (Driver *drvthis)
{
	return width;
}

/////////////////////////////////////////////////////////////////
// Returns the display height
//
MODULE_EXPORT int
mi6k_height (Driver *drvthis)
{
	return height;
}

//////////////////////////////////////////////////////////////////
// Flushes all output to the lcd...
//
MODULE_EXPORT void
mi6k_flush (Driver * drvthis)
{
	/* Move the cursor to its home position */
	fputc(0x16, dev);

	/* Write the whole framebuffer */
	fwrite(framebuf, height, width, dev);
	fflush(dev);
}

/////////////////////////////////////////////////////////////////
// Prints a character on the lcd display, at position (x,y).  The
// upper-left is (1,1), and the lower right should be (20,2).
//
MODULE_EXPORT void
mi6k_chr (Driver * drvthis, int x, int y, char c)
{
	y--;
	x--;

	if (c < 10 && c >= 0)
		c += 246;

	framebuf[(y * width) + x] = c;
}

/////////////////////////////////////////////////////////////////
// Sets the backlight on or off
//
MODULE_EXPORT void
mi6k_backlight (Driver * drvthis, int on)
{
	if (on) {
		fprintf(dev, "%c%c%c%c", 0x19, 0x30, 0xFF, brightness);
	} else {
		fprintf(dev, "%c%c%c%c", 0x19, 0x30, 0xFF, offbrightness);
	}
}

/////////////////////////////////////////////////////////////////
// Get rid of the blinking cursor
//
static void
mi6k_hidecursor ()
{
	fputc(0x0E, dev);
}

/////////////////////////////////////////////////////////////////
// Sets up for vertical bars.
//
static void
mi6k_init_vbar (Driver * drvthis)
{
	char a[] = {0x10,0x48,0x20,0x81,0x04};
	char b[] = {0x14,0x5A,0x68,0xA1,0x05};
	char c[] = {0x95,0x5E,0x7A,0xE9,0x05};
	char d[] = {0xB5,0xDF,0x7A,0xFB,0x0D};


	if (custom != vbar) {
		mi6k_set_char (drvthis, 1, a);
		mi6k_set_char (drvthis, 2, b);
		mi6k_set_char (drvthis, 3, c);
		mi6k_set_char (drvthis, 4, d);
		custom = vbar;
	}
}

/////////////////////////////////////////////////////////////////
// Draws a vertical bar...
//
MODULE_EXPORT void
mi6k_vbar (Driver * drvthis, int x, int y, int len, int promille, int options)
{
	/* x and y are the start position of the bar.
	 * The bar by default grows in the 'up' direction
	 * (other direction not yet implemented).
	 * len is the number of characters that the bar is long at 100%
	 * promille is the number of promilles (0..1000) that the bar should be filled.
	 */

	mi6k_init_vbar(drvthis);

	lib_vbar_static(drvthis, x, y, len, promille, options, cellheight, 0xF6);

}

/////////////////////////////////////////////////////////////////
// Draws a horizontal bar to the right.
//
MODULE_EXPORT void
mi6k_hbar (Driver * drvthis, int x, int y, int len, int promille, int options)
{
	/* x and y are the start position of the bar.
	 * The bar by default grows in the 'right' direction
	 * (other direction not yet implemented).
	 * len is the number of characters that the bar is long at 100%
	 * promille is the number of promilles (0..1000) that the bar should be filled.
	 */

	lib_hbar_static(drvthis, x, y, len, promille, options, cellwidth, 0xEA);
}


/////////////////////////////////////////////////////////////////
// Sets a custom character from 0-7...
//
// For input, values > 0 mean "on" and values <= 0 are "off".
//
// The input is just an array of characters...
//
MODULE_EXPORT void
mi6k_set_char (Driver * drvthis, int n, char *dat)
{
	if (n < 0 || n > 9)
		return;
	if (!dat)
		return;

	fprintf(dev, "%c%c", 0x18, 0xF6 + n);
	fwrite(dat, 1, 5, dev);
}

/////////////////////////////////////////////////////////////////
// Places an icon on screen
//
MODULE_EXPORT int
mi6k_icon (Driver * drvthis, int x, int y, int icon)
{

	char eheart[] = {40,42,49,85,5};
	char fheart[] =	{45,59,51,127,13};
//	char ellipsis[] = {136,8,0,0,0};

	if (custom == bign)
		custom = beat;

	switch( icon ) {
		case ICON_BLOCK_FILLED:
			mi6k_chr( drvthis, x, y, 127 );
			break;
		case ICON_HEART_FILLED:
			mi6k_set_char( drvthis, 9, fheart );
			mi6k_chr( drvthis, x, y, 9 );
			break;
		case ICON_HEART_OPEN:
			mi6k_set_char( drvthis, 8, eheart );
			mi6k_chr( drvthis, x, y, 8 );
			break;
		default:
			return -1;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////
// Clears the LCD screen
//
MODULE_EXPORT void
mi6k_clear (Driver * drvthis)
{
	memset (framebuf, ' ', width * height);

}

/////////////////////////////////////////////////////////////////
// Prints a string on the lcd display, at position (x,y).  The
// upper-left is (1,1), and the lower right should be (20,4).
//
MODULE_EXPORT void
mi6k_string (Driver * drvthis, int x, int y, char string[])
{
	int i;

	x -= 1;							  // Convert 1-based coords to 0-based...
	y -= 1;

	for (i = 0; string[i]; i++) {
		// Check for buffer overflows...
		if ((y * width) + x + i > (width * height))
			break;
		framebuf[(y * width) + x + i] = string[i];
	}
}

/* The End */
