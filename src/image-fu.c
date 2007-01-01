/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * image-fu.c - Imaging utilities for manipulating GdkPixbufs
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2007 David Trowbridge and Micah Dowty
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
	    
	    if (a0 != 0xFF) {
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
	}
	row += rowstride;
    }
}

void  image_draw_hline(GdkPixbuf *img, int x, int y, int width, guint32 color)
{
    guint32 *pixels = (guint32*) gdk_pixbuf_get_pixels(img);
    int rowstride = gdk_pixbuf_get_rowstride(img);
    int i;
    g_assert(gdk_pixbuf_get_n_channels(img) == 4);
    rowstride >>= 2;
    pixels += x + y * rowstride;

    for (i=width; i; i--) {
	*pixels = color;
	pixels++;
    }
}

void  image_draw_vline(GdkPixbuf *img, int x, int y, int height, guint32 color)
{
    guint32 *pixels = (guint32*) gdk_pixbuf_get_pixels(img);
    int rowstride = gdk_pixbuf_get_rowstride(img);
    int i;
    g_assert(gdk_pixbuf_get_n_channels(img) == 4);
    rowstride >>= 2;
    pixels += x + y * rowstride;

    for (i=height; i; i--) {
	*pixels = color;
	pixels += rowstride;
    }
}

void  image_draw_rect_outline(GdkPixbuf *img, int x, int y, int w, int h, guint32 color)
{
    image_draw_hline(img, x, y, w, color);
    image_draw_hline(img, x, y+h-1, w, color);
    image_draw_vline(img, x, y, h, color);
    image_draw_vline(img, x+w-1, y, h, color);
}

void  image_add_thumbnail_frame(GdkPixbuf *img)
{
    guint32 outline     = IMAGEFU_COLOR(0xFF, 0x55, 0x55, 0x55);
    guint32 transparent = IMAGEFU_COLOR(0x00, 0xFF, 0xFF, 0xFF);
    guint32 shadow      = IMAGEFU_COLOR(0x22, 0x00, 0x00, 0x00);
    int width = gdk_pixbuf_get_width(img);
    int height = gdk_pixbuf_get_height(img);
    if (width <= 2)
	return;
    if (height <= 2)
	return;

    image_draw_rect_outline(img, 0, 0, width, height, transparent);
    image_draw_rect_outline(img, 1, 1, width-2, height-2, outline);
    image_draw_hline(img, 2, height-1, width-2, shadow);
    image_draw_vline(img, width-1, 2, height-2, shadow);
}

void  image_adjust_levels(GdkPixbuf *img)
{
    guchar *pixels = gdk_pixbuf_get_pixels(img);
    int width = gdk_pixbuf_get_width(img);
    int height = gdk_pixbuf_get_height(img);
    int rowstride = gdk_pixbuf_get_rowstride(img);
    guchar *row, *pixel;
    int x, y;
    guchar r,g,b;
    guchar min, max;
    gint scale, bias;

    g_assert(gdk_pixbuf_get_n_channels(img) == 4);

    /* First pass through the image, take min/max */
    min = 0xFF;
    max = 0x00;
    row = pixels;
    for (y=0; y<height; y++) {
	pixel = row;
	for (x=0; x<width; x++) {

	    r = *(pixel++);
	    min = MIN(min, r);
	    max = MAX(max, r);

	    g = *(pixel++);
	    min = MIN(min, g);
	    max = MAX(max, g);

	    b = *(pixel++);
	    min = MIN(min, b);
	    max = MAX(max, b);

	    pixel++;
	}
	row += rowstride;
    }

    /* Calculate a scale (in fixed point) and bias */
    if (max <= min)
	return;
    bias = -min;
    scale = (255 << 16) / (max - min);

    /* Second pass, applying the scale and bias */
    row = pixels;
    for (y=0; y<height; y++) {
	pixel = row;
	for (x=0; x<width; x++) {

	    *pixel = (((*pixel) + bias) * scale) >> 16;
	    pixel++;
	    *pixel = (((*pixel) + bias) * scale) >> 16;
	    pixel++;
	    *pixel = (((*pixel) + bias) * scale) >> 16;
	    pixel++;
	    pixel++;
	}
	row += rowstride;
    }

}

/* The End */
