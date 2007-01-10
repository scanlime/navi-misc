/*
 * image-utils.h - Utility functions for dealing with images
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

#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <config.h>
#include <gdk/gdk.h>

#ifdef USE_XLIB

#include <gdk/gdkx.h>
#include <X11/Xlib.h>

void shade_ximage_generic (GdkVisual *visual, XImage *ximg, int bpl, int w, int h, int rm, int gm, int bm, int bg);
void shade_ximage_15 (void *data, int bpl, int w, int h, int rm, int gm, int bm, int bg);
void shade_ximage_16 (void *data, int bpl, int w, int h, int rm, int gm, int bm, int bg);
void shade_ximage_24 (void *data, int bpl, int w, int h, int rm, int gm, int bm, int bg);
void shade_ximage_32 (void *data, int bpl, int w, int h, int rm, int gm, int bm, int bg);
void shade_image (GdkVisual *visual, void *data, int bpl, int bpp, int w, int h, int rm, int gm, int bm, int bg, int depth);

#endif /* USE_XLIB */

#endif /* IMAGE_UTILS_H */
