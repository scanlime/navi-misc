/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * image-fu.c - Imaging utilities for manipulating GdkPixbufs
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2005 David Trowbridge and Micah Dowty
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

#include "image-fu.h"

void  image_add_checkerboard(GdkPixbuf *img)
{
    guchar *pixels = gdk_pixbuf_get_pixels(img);
    int width = gdk_pixbuf_get_width(img);
    int height = gdk_pixbuf_get_height(img);
    int rowstride = gdk_pixbuf_get_rowstride(img);
    guchar *row, *pixel;
    int x, y;
    guint r0, g0, b0, a0, z;

    g_assert(gdk_pixbuf_get_n_channels(img) == 4);

    row = pixels;
    for (y=0; y<height; y++) {
	pixel = row;
	for (x=0; x<width; x++) {
	    r0 = *(pixel++);
	    g0 = *(pixel++);
	    b0 = *(pixel++);
	    a0 = *(pixel++);
	    
	    if ((x ^ y) & CHECKERBOARD_TILE_SIZE)
		z = 0xAA * (255 - a0);
	    else
		z = 0x55 * (255 - a0);

	    pixel -= 4;
	    *(pixel++) = ((r0 * a0) + z) >> 8;
	    *(pixel++) = ((g0 * a0) + z) >> 8;
	    *(pixel++) = ((b0 * a0) + z) >> 8;
	    *(pixel++) = 0xFF;
	}
	row += rowstride;
    }

}

void  image_add_thumbnail_frame(GdkPixbuf *img)
{


}

void  image_adjust_levels(GdkPixbuf *img)
{
}

/* The End */
