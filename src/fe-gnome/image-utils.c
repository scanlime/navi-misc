/*
 * image-utils.c - Utility functions for dealing with images
 *
 * Copyright (C) 1998 Peter Zelezny
 * Copyright (C) 2006 xchat-gnome team.
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
#include "image-utils.h"

#ifdef USE_XLIB

/* Show generic routine, for pixel depths we don't know about */
void
shade_ximage_generic (GdkVisual *visual,
                      XImage    *ximg,
                      int        bpl,
                      int        w,
                      int        h,
                      int        rm,
                      int        gm,
                      int        bm,
                      int        bg)
{
	int x, y;
	int bgr = (256 - rm) * (bg & visual->red_mask);
	int bgg = (256 - gm) * (bg & visual->green_mask);
	int bgb = (256 - bm) * (bg & visual->blue_mask);

	for (x = 0; x < w; x++) {
		for (y = 0; y < h; y++) {
			unsigned long pixel = XGetPixel (ximg, x, y);
			int r = rm * (pixel & visual->red_mask) + bgr;
			int g = gm * (pixel & visual->green_mask) + bgg;
			int b = bm * (pixel & visual->blue_mask) + bgb;

			XPutPixel (ximg, x, y,
			           ((r >> 8) & visual->red_mask)   |
			           ((g >> 8) & visual->green_mask) |
			           ((b >> 8) & visual->blue_mask));
		}
	}
}

#endif /* USE_XLIB */
