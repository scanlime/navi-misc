/*
 * image-utils.c - Utility functions for dealing with images
 *
 * Copyright (C) 1998 Peter Zelezny
 * Copyright (C) 2006-2007 xchat-gnome team.
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
#include <config.h>
#include "image-utils.h"
#ifdef USE_MMX
#include "mmx_cmod.h"
#endif

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

/* "Fast" shading routine. Based on code by Willem Monsuwe <willem@stack.nl> */
#define SHADE_IMAGE(bytes, type, rmask, gmask, bmask)            \
	unsigned char *ptr;                                      \
	int x, y;                                                \
	int bgr = (256 - rm) * (bg & rmask);                     \
	int bgg = (256 - gm) * (bg & gmask);                     \
	int bgb = (256 - bm) * (bg & bmask);                     \
	ptr = (unsigned char *) data + (w * bytes);              \
	for (y = h; --y >= 0;) {                                 \
		for (x = -w; x < 0; x++) {                       \
			int r, g, b;                             \
			b = ((type *) ptr)[x];                   \
			r = rm * (b & rmask) + bgr;              \
			g = gm * (b & gmask) + bgg;              \
			b = bm * (b & bmask) + bgb;              \
			((type *) ptr)[x] = ((r >> 8) & rmask) | \
			                    ((g >> 8) & gmask) | \
			                    ((b >> 8) & bmask);  \
		}                                                \
		ptr += bpl;                                      \
	}

void
shade_ximage_15 (void *data, int bpl, int w, int h, int rm, int gm, int bm, int bg)
{
	SHADE_IMAGE (2, guint16, 0x7c00, 0x3e0, 0x1f);
}

void
shade_ximage_16 (void *data, int bpl, int w, int h, int rm, int gm, int bm, int bg)
{
	SHADE_IMAGE (2, guint16, 0xf800, 0x7e0, 0x1f);
}

void
shade_ximage_24 (void *data, int bpl, int w, int h, int rm, int gm, int bm, int bg)
{
	/* 24 has to be a special case, there's no guint24, or 24bit MOV :) */
	unsigned char *ptr;
	int x, y;
	int bgr = (256 - rm) * ((bg & 0xff0000) >> 16);
	int bgg = (256 - gm) * ((bg & 0xff00) >> 8);
	int bgb = (256 - bm) * (bg & 0xff);

	ptr = (unsigned char *) data + (w * 3);
	for (y = h; --y >= 0;) {
		for (x = -(w * 3); x < 0; x += 3) {
			int r, g, b;

#if (G_BYTE_ORDER == G_BIG_ENDIAN)
			r = (ptr[x + 0] * rm + bgr) >> 8;
			g = (ptr[x + 1] * gm + bgg) >> 8;
			b = (ptr[x + 2] * bm + bgb) >> 8;
			ptr[x + 0] = r;
			ptr[x + 1] = g;
			ptr[x + 2] = b;
#else /* (G_BYTE_ORDER == G_BIG_ENDIAN) */
			r = (ptr[x + 2] * rm + bgr) >> 8;
			g = (ptr[x + 1] * gm + bgg) >> 8;
			b = (ptr[x + 0] * bm + bgb) >> 8;
			ptr[x + 2] = r;
			ptr[x + 1] = g;
			ptr[x + 0] = b;
#endif /* (G_BYTE_ORDER == G_BIG_ENDIAN) */
		}
		ptr += bpl;
	}
}

void
shade_ximage_32 (void *data, int bpl, int w, int h, int rm, int gm, int bm, int bg)
{
	SHADE_IMAGE (4, guint32, 0xff0000, 0xff00, 0xff);
}

void
shade_image (GdkVisual *visual, void *data, int bpl, int bpp, int w, int h, int rm, int gm, int bm, int bg, int depth)
{
	int bg_r = bg & visual->red_mask;
	int bg_g = bg & visual->green_mask;
	int bg_b = bg & visual->blue_mask;

#ifdef USE_MMX
	if (bg_r == 0 && bg_g == 0 && bg_b == 0 && have_mmx ()) {
		switch (depth) {
		case 15:
			shade_ximage_15_mmx (data, bpl, w, h, rm, gm, bm);
			return;

		case 16:
			shade_ximage_16_mmx (data, bpl, w, h, rm, gm, bm);
			return;

		case 24:
			if (bpp != 32) {
				break;
			}

		case 32:
			shade_ximage_32_mmx (data, bpl, w, h, rm, gm, bm);
			return;

		default:
			break;
		}
	}
#endif /* USE_MMX */

	switch (depth) {
	case 15:
		shade_ximage_15 (data, bpl, w, h, rm, gm, bm, bg);
		break;
	case 16:
		shade_ximage_16 (data, bpl, w, h, rm, gm, bm, bg);
		break;
	case 24:
		if (bpp != 32) {
			shade_ximage_24 (data, bpl, w, h, rm, gm, bm, bg);
			break;
		}

	case 32:
		shade_ximage_32 (data, bpl, w, h, rm, gm, bm, bg);
		break;
	}
}


#endif /* USE_XLIB */
