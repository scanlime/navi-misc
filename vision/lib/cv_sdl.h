/*
 * cv_sdl.h - Glue to make it easy to display video from OpenCV applications
 *            in SDL, taking advantage of hardware accelerated YUV overlays.
 *
 * Copyright (C) 2004 Micah Dowty <micah@navi.cx>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __CV_SDL__H_
#define __CV_SDL__H_

#include <cv.h>
#include <SDL.h>

/* Show a single YUV image in the window, initializing and/or resizing as necessary */
void cv_sdl_show_yuv_image(IplImage *image);

/* Show several YUV images, tiled, in one window. Assumes all images are the same size, for now */
void cv_sdl_show_yuv_tiles(IplImage **images, int num_images, int width);

/* Process pending events, returns zero if it's time to quit */
int cv_sdl_process_events();

#endif /* __CV_SDL__H_ */

/* The End */
